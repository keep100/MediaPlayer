#include "SynModule.h"
extern "C" {
#include "libavutil/time.h"
}
#include <QDebug>
void SynModule::doTask() {
    connect(timer, &QTimer::timeout, [this](){
        std::shared_ptr<YUVData> static vtemp = yuvQueue.dequeue();
        std::shared_ptr<PCMData> static atemp = pcmQueue.dequeue();

        a_clock_t = atemp->pts * a_time_base_d - ap2->GetNoPlayMs() / 1000.0;
        // 播放音频
        // 音频无需跟其他时间同步
        if (ap2->GetFree()>=atemp->size) {
            ap2->Write(atemp->data, atemp->size);
            atemp = pcmQueue.dequeue();
        }

        // 视频以音频时间为依据来同步
        v_clock_t = vtemp->pts * v_time_base_d;
        // 播放视频
        // 视频不超过音频0.01s则播放
        if (v_clock_t <= (a_clock_t + 0.01)) {
            // 视频比音频慢超过0.05s则跳帧
            while (v_clock_t < (a_clock_t - 0.04)) {
                vtemp = yuvQueue.dequeue();
                v_clock_t = vtemp->pts * v_time_base_d;
            }
            emit transmitYUV(vtemp, int64_t(a_clock_t * 1000));
            vtemp = yuvQueue.dequeue();
        }
    });
    timer->start(5);
}
