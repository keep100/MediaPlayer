extern "C" {
#include "libavcodec/avcodec.h"
#include "libavdevice/avdevice.h"
#include "libavformat/avformat.h"
#include "libavformat/avio.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
}

#include "XDecode.h"
#include "XVideoThread.h"
#include <iostream>
//打开，不管成功与否都清理
bool XVideoThread::Open(AVCodecParameters *para, IVideoCall *call, int width,
                        int height) {
    if (!para) {
        return false;
    }
    Clear();
    vmux.lock();
    synpts = 0;
    //初始化显示窗口
    //    this->call = call;
    //    if (call)
    //    {
    //        call->Init(width, height);
    //    }
    codecParam = para;
    vmux.unlock();
    int re = true;
    yuvQueue.init();
    if (!decode->Open(para)) {
        std::cout << "video XDecode open failed!" << std::endl;
        re = false;
    }

    std::cout << "XVideoThread::Open :" << re << std::endl;
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
        // cout << "synpts = " << synpts << " dpts =" << decode->pts << endl;
        if (this->isPause) {
            vmux.unlock();
            msleep(5);
            continue;
        }
        std::cout << "Video...\n";
        //音视频同步
        if (synpts > 0 && synpts < decode->pts) {
            vmux.unlock();
            std::cout << "syn\n";
            msleep(1);
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
            if (!frame) {
                break;
            }
            // 显示视频
            std::shared_ptr<YUVData> yuvFrame = convertToYUV420P(frame);
            yuvQueue.enqueue(yuvFrame);
            //            if (call)
            //                call->Repaint(frame);
            qDebug() << "send--------------------------";
            //            showImage->sendimage(frameToImage(frame));
            av_frame_free(&frame);
            msleep(1);
        }
        vmux.unlock();
    }
    avcodec_parameters_free(&codecParam);
    std::cout << "videoTread exit \n";
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

std::shared_ptr<YUVData> XVideoThread::getYUVData() {
    return yuvQueue.tryDequeue();
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
        if (call) {
            call->Repaint(frame);
        }
        vmux.unlock();
        return 1;
    }
    XFreeFrame(&frame);
    vmux.unlock();
    return false;
}
XVideoThread::XVideoThread() {
    showImage = new ShowImage();
}

XVideoThread::~XVideoThread() {
    std::cout << "XVideoThread::~XVideoThread()\n";
    isExit = true;
    wait();
}
