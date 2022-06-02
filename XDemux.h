#pragma once
#ifndef XDEMUX_H
#define XDEMUX_H

struct AVFormatContext;
struct AVPacket;
struct AVCodecParameters;
struct AVRational;
#include <mutex>
#include "SynModule.h"
class XDemux: public QObject
{
    Q_OBJECT
public:
    XDemux();
    virtual ~XDemux();
    virtual bool Open(const char *url);
    virtual AVPacket *Read();

    virtual bool isAudio(AVPacket *pkt);
    //获取视频参数  返回的空间需要清理  avcodec_parameters_free
    AVCodecParameters *CopyVPara();

    //获取音频参数  返回的空间需要清理 avcodec_parameters_free
    AVCodecParameters *CopyAPara();

    //获取音频参数  返回的空间需要清理 avcodec_parameters_free
    AVCodecParameters *CopySPara();

    //seek 位置 pos 0.0 ~1.0
    virtual bool Seek(double pos);

    //清空读取缓存
    virtual void Clear();
    virtual void Close();

    virtual AVPacket *ReadVideo();

    AVRational getVTimebase() {
        AVRational temp{0, 1};
        if (ic != nullptr && videoStream >= 0){
            temp =  ic->streams[videoStream]->time_base;
        }
        return temp;
    }

    AVRational getATimebase() {
        AVRational temp{0,1};
        if (ic != nullptr && audioStream >= 0)
            temp = ic->streams[audioStream]->time_base;
        return temp;
    }

    bool hasAudio() {
        return (audioStream >= 0);
    }

    bool isSubtitle(AVPacket *pkt);

    //总时长 毫秒
    int totalMs = 0;
    int width = 0;
    int height = 0;
    //音频信息
    int sampleRate = 0;
    int channels = 0;
    int sampleFormat = 2;


protected:
    std::mutex mtx;
    AVFormatContext *ic = NULL;
    int videoStream = -1;
    int audioStream = -1;
    int subtitleStream = -1;
signals:
    void transmitTime(int64_t time);

};

#endif // XDEMUX_H
