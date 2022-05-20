extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
#include "libavformat/avio.h"
#include "libavutil/imgutils.h"
}

#include "XVideoThread.h"
#include "XDecode.h"
#include <iostream>
using namespace std;
//打开，不管成功与否都清理
bool XVideoThread::Open(AVCodecParameters *para, IVideoCall *call, int width, int height)
{
    if (!para)
    {
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
    if (!decode->Open(para))
    {
        cout << "video XDecode open failed!" << endl;
        re = false;
    }

    cout << "XVideoThread::Open :" << re << endl;
    return re;
}
void XVideoThread::SetPause(bool isPause)
{
    vmux.lock();
    this->isPause = isPause;
    vmux.unlock();
}
void XVideoThread::run()
{
    while (!isExit)
    {
        vmux.lock();
        //cout << "synpts = " << synpts << " dpts =" << decode->pts << endl;
        if (this->isPause)
        {
            vmux.unlock();
            msleep(5);
            continue;
        }
        cout << "Video...\n";
        //音视频同步
        if (synpts > 0 && synpts < decode->pts)
        {
            vmux.unlock();
            cout << "syn\n";
            msleep(1);
            continue;
        }
        AVPacket *pkt = Pop();
        bool re = decode->Send(pkt);
        if (!re)
        {
            vmux.unlock();
            msleep(1);
            continue;
        }
        //一次send 多次recv
        while (!isExit)
        {
            AVFrame *frame = decode->Recv();
            if (!frame)
            {
                break;
            }
            //显示视频
//            if (call)
//            {
//                call->Repaint(frame);
//            }
            qDebug() << "send--------------------------";
            showImage->sendimage(frameToImage(frame));
            msleep(1);
        }
        vmux.unlock();
    }
    avcodec_parameters_free(&codecParam);
    cout << "videoTread exit \n";
}

QImage XVideoThread::frameToImage(AVFrame *frame)
{
    qDebug() << "frameToImage";
    QImage output(frame->width, frame->height, QImage::Format_RGB32); //构造一个QImage用作输出
    int outputLineSize[4];                                                       //构造AVFrame到QImage所需要的数据
    av_image_fill_linesizes(outputLineSize, AV_PIX_FMT_RGB32, frame->width);
    uint8_t *outputDst[] = {output.bits()};
    //构造一个格式转换上下文
    SwsContext *imgConvertContext = sws_getContext(
                                        codecParam->width, codecParam->height,
                                        (AVPixelFormat)codecParam->format, codecParam->width, codecParam->height,
                                        AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);

    //把解码得到的损坏的像素数据剔除
    sws_scale(imgConvertContext, frame->data, frame->linesize, 0, codecParam->height, outputDst, outputLineSize);
//    output.save("frameImg.jpg");
    sws_freeContext(imgConvertContext);
    av_frame_free(&frame);
    return output;
}

bool XVideoThread::RepaintPts(long long seekPts, AVPacket *pkt)
{
    vmux.lock();
    bool ret = decode->Send(pkt);
    if (!ret)
    {
        vmux.unlock();
        return 1; //表示结束解码
    }
    AVFrame *frame = decode->Recv();
    if (!frame)
    {
        vmux.unlock();
        return 0;
    }
    //到达位置
    if (decode->pts >= seekPts)
    {
        if (call)
        {
            call->Repaint(frame);
        }
        vmux.unlock();
        return 1;
    }
    XFreeFrame(&frame);
    vmux.unlock();
    return false;
}
XVideoThread::XVideoThread()
{
    showImage = new ShowImage();
}


XVideoThread::~XVideoThread()
{
    cout << "XVideoThread::~XVideoThread()\n";
    isExit = true;
    wait();
}
