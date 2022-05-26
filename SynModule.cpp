#include "SynModule.h"
#include <QDebug>
#include "controller.h"
SynModule::SynModule(): yuvQueue(yuvBuffSize){
    QObject::connect(this, &SynModule::transmitYUV, Controller::controller, &Controller::onUpdate);
}
SynModule::~SynModule() {
    clear();
    QObject::disconnect(this, &SynModule::transmitYUV, Controller::controller, &Controller::onUpdate);
}

void SynModule::run() {
    std::shared_ptr<YUVData> vtemp = yuvQueue.dequeue();
    double start_time = av_gettime() / 1000000.0;
    while (true) {
        // 若没有音频，则用系统时钟来同步视频时间
        if (!hasAudio)
            syn_clock_t = av_gettime() / 1000000.0 - start_time;
        v_clock_t = vtemp->pts * v_time_base_d;
//        qDebug() <<"SynModule" <<  v_clock_t << syn_clock_t;
        // 播放视频
        // 视频不超过音频0.01s则播放
        if (v_clock_t <= (syn_clock_t + 0.1)) {
             // 视频比音频慢超过0.1s则丢帧
            while (v_clock_t < (syn_clock_t - 0.3)) {
                vtemp = yuvQueue.dequeue();
                v_clock_t = vtemp->pts * v_time_base_d;
            }
            emit transmitYUV(vtemp, int64_t(syn_clock_t * 1000));
            vtemp = yuvQueue.dequeue();
        } else {
            int64_t delay = (v_clock_t - syn_clock_t) * 1000;
            while (delay > 1000) {
                vtemp = yuvQueue.dequeue();
                emit transmitYUV(vtemp, int64_t(syn_clock_t * 1000));
                v_clock_t = vtemp->pts * v_time_base_d;
                delay = (v_clock_t - syn_clock_t - 0.02) * 1000;
            }
            // delay最多为5ms
            if (delay > 5 || delay < 0)
                delay = 5;
            msleep(delay);
        }
    }
}

