#pragma once
#ifndef SYNMODULE_H
#define SYNMODULE_H
#include "audioPlay2.h"
#include "util/bufferqueue.h"
#include "util/pcmdata.h"
#include "util/yuvdata.h"
#include <QTimer>
#include <QThread>
#include <QMutex>

// 音视频缓存队列的容量
const static size_t yuvBuffSize = 50;

class SynModule : public QThread {
    Q_OBJECT
private:
    // 视频帧缓冲队列
    BufferQueue<std::shared_ptr<YUVData>> yuvQueue;
    std::shared_ptr<YUVData> vtemp;
    // 视频时钟，同步到音频时钟
    double v_clock_t = 0;
    // 音频时钟
    double a_clock_t = 0;
    // 无音频时用的同步时间
    double syn_clock_t = 0;
    // 音视频帧的pts对应的timebase
    double v_time_base_d;
    double a_time_base_d;
public:
    bool hasAudio;

public:
    SynModule();
    ~SynModule();
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
    // 将音频时钟赋值给syn_clock_t
    void setAClock(int64_t pts, long long noPlayMs) {
        a_clock_t = pts * a_time_base_d - noPlayMs / 1000.0;
    }
    // 重置同步模块
    void clear();
    void run();

signals:
    // 通知前端渲染下一帧
    void transmitYUV(std::shared_ptr<YUVData> data);
    // 发送当前播放时间
    void transmitTime(int64_t time);

};

#endif // SYNMODULE_H
