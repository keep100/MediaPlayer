﻿#include <QThread>
#include <mutex>
#include <list>
struct AVCodecParameters;
#include "XDecodeThread.h"
#include "audioPlay2.h"
#include "SynModule.h"
#include "XAudioResample.h"
class XAudioThread:public XDecodeThread
{
    Q_OBJECT
public:
    //当前音频播放的pts
    long long pts = 0;
    //打开，不管成功与否都清理
    virtual bool Open(AVCodecParameters *para,int sampleRate,int channels, SynModule *syn);
    //停止线程，清理资源
    virtual void Close();
    virtual void Clear();
    void run();
    XAudioThread();
    virtual ~XAudioThread();
    void SetPause(bool isPause);
    void SetVolume(double volume);
    bool isPause = false;
    bool interupt = false;
    // 是否有视频流，若没有则在当前线程发送播放时间
    bool hasVideo;
    // 音频流的timebase
    double a_time_base_d;


protected:
    std::mutex amux;
    audioPlay2 *ap2 = 0;
    XAudioResample *rsmp = 0;
    SynModule *syn = 0;

signals:
    void transmitTime(std::shared_ptr<YUVData>, int64_t);
};
