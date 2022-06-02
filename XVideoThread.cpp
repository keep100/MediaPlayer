extern "C" {
#include "libavcodec/avcodec.h"
#include "libavdevice/avdevice.h"
#include "libavformat/avformat.h"
#include "libavformat/avio.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
}
#include "SynModule.h"
#include "XDecode.h"
#include "XVideoThread.h"
#include <iostream>
//打开，不管成功与否都清理
bool XVideoThread::Open(AVCodecParameters *para, IVideoCall *call, int width,
                        int height, SynModule *syn) {
    if (!para)
        return false;
    Clear();
    vmux.lock();
    synpts = 0;
    codecParam = para;
    vmux.unlock();
    int re = true;
    if (!decode->Open(para))
        re = false;

    this->syn = syn;
    return re;
}
void XVideoThread::SetPause(bool isPause) {
    vmux.lock();
    this->isPause = isPause;
    vmux.unlock();
}

void XVideoThread::run() {
    while (!isExit) {
        vmux.lock();
        if (this->isPause) {
            vmux.unlock();
            msleep(5);
            continue;
        }
        AVPacket *pkt = Pop();
        bool re = decode->Send(pkt);
        if (!re) {
            vmux.unlock();
            msleep(1);
            continue;
        }
        //一次send 多次recv
        while (!isExit) {
            AVFrame *frame = decode->Recv();
            if (!frame)
                break;
            std::shared_ptr<YUVData> t;
            //如果字幕成功打开，且是文本字幕，需经过过滤器，则输出使用subtitle
            // filter过滤后的图像
            if (subtitleOpened) {
                qDebug() << "subtitleOpened";
                AVFrame *filter_frame = av_frame_alloc();
                if (av_buffersrc_add_frame_flags(buffersrcContext, frame,
                                                 AV_BUFFERSRC_FLAG_KEEP_REF) < 0)
                    break;
                int ret = av_buffersink_get_frame(buffersinkContext, filter_frame);
                if (ret < 0)
                    break;
                t = convertToYUV420P(filter_frame);
                av_frame_unref(filter_frame);
            }
            // 字幕打开失败或者没有文本字幕
            else {
                t = convertToYUV420P(frame);
            }
            if ((t->pts = frame->best_effort_timestamp) == AV_NOPTS_VALUE)
                t->pts = 0;
            // 缓冲区满了则睡眠等待，但判断不超过10次
            int i = 0;
            while (!syn->pushYuv(t) && i++ < 10) {
                vmux.unlock();
                msleep(1);
                vmux.lock();
            }
            av_frame_free(&frame);
        }
        vmux.unlock();
    }
    avcodec_parameters_free(&codecParam);
    qDebug() << "videoTread exit";
}

QImage XVideoThread::frameToImage(AVFrame *frame) {
    qDebug() << "frameToImage";
    QImage output(frame->width, frame->height,
                  QImage::Format_RGB32); //构造一个QImage用作输出
    int outputLineSize[4]; //构造AVFrame到QImage所需要的数据
    av_image_fill_linesizes(outputLineSize, AV_PIX_FMT_RGB32, frame->width);
    uint8_t *outputDst[] = {output.bits()};
    //构造一个格式转换上下文
    SwsContext *imgConvertContext = sws_getContext(
                codecParam->width, codecParam->height, (AVPixelFormat)codecParam->format,
                codecParam->width, codecParam->height, AV_PIX_FMT_RGB32, SWS_BICUBIC,
                NULL, NULL, NULL);

    //把解码得到的损坏的像素数据剔除
    sws_scale(imgConvertContext, frame->data, frame->linesize, 0,
              codecParam->height, outputDst, outputLineSize);
    //    output.save("frameImg.jpg");
    sws_freeContext(imgConvertContext);
    av_frame_free(&frame);
    return output;
}

// 将视频帧转成YUV420P格式
std::shared_ptr<YUVData> XVideoThread::convertToYUV420P(AVFrame *videoFrame) {
    std::shared_ptr<YUVData> yuvVideo = std::make_shared<YUVData>();

    int dst_linesize[4];
    uint8_t *dst_data[4];

    av_image_alloc(dst_data, dst_linesize, videoFrame->width, videoFrame->height,
                   AV_PIX_FMT_YUV420P, 1);
    SwsContext *swsContext = sws_getContext(
                videoFrame->width, videoFrame->height, (AVPixelFormat)videoFrame->format,
                videoFrame->width, videoFrame->height, AV_PIX_FMT_YUV420P, SWS_BILINEAR,
                nullptr, nullptr, nullptr);
    sws_scale(swsContext, videoFrame->data, videoFrame->linesize, 0,
              videoFrame->height, dst_data, dst_linesize);
    sws_freeContext(swsContext);

    memcpy(yuvVideo->Y, dst_data[0], dst_linesize[0] * videoFrame->height);
    memcpy(yuvVideo->U, dst_data[1], dst_linesize[1] * videoFrame->height>>1);
    memcpy(yuvVideo->V, dst_data[2], dst_linesize[2] * videoFrame->height>>1);

    yuvVideo->yWidth =  videoFrame->width;
    yuvVideo->yHeight = videoFrame->height;
    yuvVideo->uWidth =  videoFrame->width>>1;
    yuvVideo->uHeight = videoFrame->height >> 1;
    yuvVideo->vWidth =  videoFrame->width>>1;
    yuvVideo->vHeight = videoFrame->height >> 1;

    av_freep(dst_data);
    return yuvVideo;
}

bool XVideoThread::RepaintPts(long long seekPts, AVPacket *pkt) {
    vmux.lock();
    bool ret = decode->Send(pkt);
    if (!ret) {
        vmux.unlock();
        return 1; //表示结束解码
    }
    AVFrame *frame = decode->Recv();

    if (!frame) {
        vmux.unlock();
        return 0;
    }

    //到达位置
    if (decode->pts >= seekPts) {
        std::shared_ptr<YUVData> t = convertToYUV420P(frame);
        if ((t->pts = frame->best_effort_timestamp) == AV_NOPTS_VALUE)
            t->pts = 0;
        // 缓冲区满了则睡眠等待
        while (!syn->pushYuv(t)) {
            vmux.unlock();
            msleep(1);
            vmux.lock();
        }
        vmux.unlock();
        return 1;
    }
    XFreeFrame(&frame);
    vmux.unlock();
    return 0;
}

XVideoThread::XVideoThread(){
    showImage = new ShowImage();
}

XVideoThread::~XVideoThread() {
    qDebug() << "XVideoThread::~XVideoThread()";
    if (buffersrcContext != nullptr)
        avfilter_free(buffersrcContext);
    if (buffersinkContext != nullptr)
        avfilter_free(buffersinkContext);
    isExit = true;
    wait();
}
