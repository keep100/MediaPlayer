#include "SynModule.h"
#include <QDebug>
#include "controller.h"
SynModule::SynModule(): yuvQueue(yuvBuffSize){
    QObject::connect(this, &SynModule::transmitYUV, Controller::controller, &Controller::updateYUV);
    if (!hasAudio)
        QObject::connect(this, &SynModule::transmitTime, Controller::controller, &Controller::updateTime);
}
SynModule::~SynModule() {
    clear();
    QObject::disconnect(this, &SynModule::transmitYUV, Controller::controller, &Controller::updateYUV);
    if (!hasAudio)
        QObject::disconnect(this, &SynModule::transmitTime, Controller::controller, &Controller::updateTime);
}

void SynModule::clear() {
    yuvQueue.init();
    emit transmitYUV(nullptr);
}

void SynModule::run() {
    vtemp = yuvQueue.dequeue();
    // 若有音频
    if (hasAudio) {
        while (true) {
            v_clock_t = vtemp->pts * v_time_base_d;
            // 播放视频
            // 视频不超过音频0.1s则播放
            if (v_clock_t <= (a_clock_t + 0.1)) {
                // 视频比音频慢超过0.3s则丢帧
                while (v_clock_t < (a_clock_t - 0.3)) {
                    vtemp = yuvQueue.dequeue();
                    v_clock_t = vtemp->pts * v_time_base_d;
                }
                emit transmitYUV(vtemp);
                vtemp = yuvQueue.dequeue();
            } else {
                // 计算视频同步音频所需的延迟
                int64_t delay = (v_clock_t - a_clock_t - 0.02) * 1000;
                while (delay > 1000) {
                    vtemp = yuvQueue.dequeue();
                    emit transmitYUV(vtemp);
                    v_clock_t = vtemp->pts * v_time_base_d;
                    delay = (v_clock_t - a_clock_t - 0.02) * 1000;
                }
                // delay最多为5ms
                if (delay > 5 || delay < 0)
                    delay = 5;
                msleep(delay);
            }
        }
    }
    // 若没有音频，则用系统时钟来同步视频时间
    else {

        double start_time = av_gettime() / 1000000.0;
        while (true) {
            // offset_time用于校正实际播放时间
            int64_t start_time_t = av_gettime() / 1000;
            int64_t offset_time;

            syn_clock_t = av_gettime() / 1000000.0 - start_time;
            v_clock_t = vtemp->pts * v_time_base_d;
            // 播放视频
            // 视频不超过音频0.1s则播放
            if (v_clock_t <= (syn_clock_t + 0.1)) {
                // 视频比音频慢超过0.3s则丢帧
                while (v_clock_t < (syn_clock_t - 0.3)) {
                    vtemp = yuvQueue.dequeue();
                    v_clock_t = vtemp->pts * v_time_base_d;
                    // 更新同步时钟
                    offset_time = av_gettime() / 1000 - start_time_t;
                    syn_clock_t += offset_time /1000.0;
                }
                offset_time = av_gettime() / 1000 - start_time_t;
                emit transmitYUV(vtemp);
                emit transmitTime(int64_t(syn_clock_t * 1000) + offset_time);
                vtemp = yuvQueue.dequeue();
            } else {
                int64_t delay = (v_clock_t - syn_clock_t - 0.02) * 1000;
                while (delay > 1000) {
                    vtemp = yuvQueue.dequeue();
                    offset_time = av_gettime() / 1000 - start_time_t;
                    emit transmitYUV(vtemp);
                    emit transmitTime(int64_t(syn_clock_t * 1000) + offset_time);
                    v_clock_t = vtemp->pts * v_time_base_d;
                    // 更新同步时钟
                    syn_clock_t += offset_time / 1000.0;
                    delay = (v_clock_t - syn_clock_t - 0.02) * 1000;
                }
                // delay最多为5ms
                if (delay > 5 || delay < 0)
                    delay = 5;
                msleep(delay);
            }
        }
    }
}

