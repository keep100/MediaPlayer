#include "XMediaManager.h"
#include "XDemuxThread.h"
#include "XDemux.h"
#include "XVideoThread.h"
#include "XAudioThread.h"
extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
#include "libavformat/avio.h"
#include "libavutil/imgutils.h"
}
#include <iostream>
#include <QThread>
#include <QDebug>
using namespace std;

BriefInfo XMediaManager::getBriefInfo(const char* url){

    BriefInfo briefInfo;
    if (url == 0 || url[0] == '\0')
        return briefInfo;
    XDemux *demux = new XDemux();
    XDecode *decode = new XDecode();
    demux->Open(url);

    briefInfo.totalMs = demux->totalMs;
    briefInfo.width = demux->width;
    briefInfo.height = demux->height;
    briefInfo.channels = demux->channels;
    briefInfo.sampleRate = demux->sampleRate;
    briefInfo.mediaType = QString(url).split(".").last();
//    qDebug()<<briefInfo.mediaType;
    qDebug()<<demux->CopyAPara();
    qDebug()<<demux->CopyVPara();
    if(QString("mp4, flv, avi, mkv").contains(briefInfo.mediaType)){//是视频就提取视频的相关信息
        AVFrame *pFrame = av_frame_alloc();
        demux->Seek(0.5);
        //先recv，后send
        while (true)
        {
            pFrame = decode->Recv();
            if (pFrame)
            {
//                qDebug() << "receive--------------------------";
                QImage temp = getQImageFromFrame(pFrame, demux->CopyVPara());
                if(!temp.isNull()&&temp.bits()){
                    int w = temp.width(), h = temp.height();
                    int rgbValue = (((temp.pixel(0.25*w, 0.25*h)+temp.pixel(0.75*w, 0.75*h)+temp.pixel(0.25*w, 0.75*h)+temp.pixel(0.75*w, 0.25*h))/4));
                    int means = (qRed(rgbValue)+qGreen(rgbValue)+qBlue(rgbValue))/3;
//                    qDebug()<<qRed(rgbValue)<<qGreen(rgbValue)<<qBlue(rgbValue)<<means;
                    if(means>150){
                        qDebug()<<"有东西的图";
                        break;
                    }

                }
//                break;
            }
//            qDebug() << "send--------------------------";
            AVPacket *pkt = demux->ReadVideo();
            decode->Send(pkt);
        }
        briefInfo.qimage = getQImageFromFrame(pFrame, demux->CopyVPara());
    }
    else if(QString("mp3, flac, wav, rm").contains(briefInfo.mediaType)){//是视频就提取视频的相关信息

    }


    return briefInfo;
}

QImage XMediaManager::getQImageFromFrame(const AVFrame* frame, AVCodecParameters *para)
{

    qDebug() << "frameToImage";
    QImage output(frame->width, frame->height, QImage::Format_RGB32); //构造一个QImage用作输出
//    qDebug()<<frame->width<<", "<<frame->height;
    int outputLineSize[4];                                            //构造AVFrame到QImage所需要的数据
    av_image_fill_linesizes(outputLineSize, AV_PIX_FMT_RGB32, para->width);
    uint8_t *outputDst[] = {output.bits()};
    //构造一个格式转换上下文
    SwsContext *imgConvertContext = sws_getContext(
                                        para->width, para->height,
                                        (AVPixelFormat)para->format, para->width, para->height,
                                        AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);

    //把解码得到的损坏的像素数据剔除
    sws_scale(imgConvertContext, frame->data, frame->linesize, 0, para->height, outputDst, outputLineSize);
//    output.save("C:\\Users\\16409\\Desktop\\oneFrame.jpg");
    sws_freeContext(imgConvertContext);
//    av_frame_free(&frame);
    return output;

}

XMediaManager::XMediaManager(){
    _curState = INITIAL;
    if(!demuxThread)
        demuxThread = new XDemuxThread();
    demuxThread->Clear();
}

bool XMediaManager::open(const char *url){
    if(_curState==INITIAL){
        _curState = READY;
        if(!demuxThread)
            demuxThread = new XDemuxThread();
        return demuxThread->Open(url, NULL);

    }
    else if(_curState==END){
        _curState = READY;
        if(!demuxThread)
            demuxThread = new XDemuxThread();
        return demuxThread->Open(url, NULL);
    }
    else{
        _curState = INITIAL;
        qDebug()<<"error at XMediaManager::open()";
        return false;
    }
    return false;
}

void XMediaManager::play(){
    if(_curState==READY){
        _curState = PLAYING;
        if(!demuxThread){
            _curState = END;
            qDebug()<<"error at XMediaManager::play()1";
            return;
        }
        demuxThread->Start();
    }
    else if(_curState==PAUSED){
        _curState = PLAYING;
        if(!demuxThread){
            _curState = END;
            qDebug()<<"error at XMediaManager::play()2";
            return;
        }
        demuxThread->SetPause(false);
    }
    else{
        _curState = INITIAL;
        qDebug()<<"error at XMediaManager::play()3";
        return;
    }

}

void XMediaManager::pause(){
    if(_curState==PLAYING){
        _curState = PAUSED;
        if(!demuxThread){
            _curState = END;
            qDebug()<<"error at XMediaManager::pause()1";
            return;
        }
        demuxThread->SetPause(true);
    }
    else{
        _curState = INITIAL;
        qDebug()<<"error at XMediaManager::pause()2";
        return;
    }

}

void XMediaManager::seek(double pos){
    if(_curState==PLAYING||_curState==PAUSED){
        if(!demuxThread){
            _curState = END;
            qDebug()<<"error at XMediaManager::seek()1";
            return;
        }
        demuxThread->Seek(pos);

    }
    else{
        _curState = INITIAL;
        qDebug()<<"error at XMediaManager::seek()2";
        return;
    }
}

void XMediaManager::end(){
    _curState = END;
    if(demuxThread){
        demuxThread->Clear();
        demuxThread->Close();
    }
    return;
}



