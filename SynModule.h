#pragma once
#ifndef SYNMODULE_H
#define SYNMODULE_H
#include "audioPlay2.h"
#include "util/bufferqueue.h"
#include "util/pcmdata.h"
#include "util/yuvdata.h"
#include <QTimer>
#include <QThread>

// 音视频缓存队列的容量
const static size_t yuvBuffSize = 50;
const static size_t pcmBuffSize = 50;

class SynModule : public QObject {
    Q_OBJECT
private:
    // 音视频帧缓冲队列
    BufferQueue<std::shared_ptr<YUVData>> yuvQueue;
    BufferQueue<std::shared_ptr<PCMData>> pcmQueue;
    // 音视频时钟，用于同步和显示当前播放时间
    double v_clock_t = 0;
    double a_clock_t = 0;
    // 音视频帧的pts对应的timebase
    double v_time_base_d;
    double a_time_base_d;
    // 音频播放器类
    audioPlay2 *ap2 = nullptr;
    // 用于定时发送数据到设备播放
    QTimer *timer = new QTimer(this);

public:
    SynModule(): yuvQueue(yuvBuffSize), pcmQueue(pcmBuffSize){
        QMediaDevices media(this);
        const QAudioDevice deviceInfo = media.defaultAudioOutput();
        ap2 = new audioPlay2();
        ap2->Open(deviceInfo);
        ap2->Clear();
    }
    ~SynModule() {
        clear();
        ap2->Close();
        delete timer;
    }

    void setVTimeBase(AVRational vtb) {
        v_time_base_d = av_q2d(vtb);
    }
    void setATimeBase(AVRational atb){
        a_time_base_d = av_q2d(atb);
    }
    // 将解码得到原始帧放进缓冲队列
    bool pushYuv(std::shared_ptr<YUVData> data) {
        return yuvQueue.tryEnqueue(data);
    }
    bool pushPcm(std::shared_ptr<PCMData> data) {
        return pcmQueue.tryEnqueue(data);
    }
    // 重置同步模块
    void clear() {
        ap2->Clear();
        yuvQueue.init();
        pcmQueue.init();
        timer->stop();
        v_clock_t = 0;
        a_clock_t = 0;
    }
    // 获取播放设备缓冲区中还未播放的音频的时间长度
    int64_t GetNoPlayMs() { return ap2->GetNoPlayMs(); }
    void SetPause(bool isPause) { ap2->SetPause(isPause); }
    void SetVolume(double volume) { ap2->SetVolume(volume); }
    QAudioFormat GetFormat() { return ap2->GetFormat(); }
    // 主要工作函数，同步音视频并将原始帧从缓冲队列发送到设备
    void doTask();
signals:
    // 通知前端渲染下一帧，并发送当前播放时间
    void transmitYUV(std::shared_ptr<YUVData> data, int64_t time);
};

#endif // SYNMODULE_H
