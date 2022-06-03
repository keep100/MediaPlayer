#ifndef XAUDIORESAMPLE_H
#define XAUDIORESAMPLE_H
#include "audioPlay2.h"
#include <QAudioDevice>
#include <QAudioSink>
#include <QVBoxLayout>
#include <qmath.h>
#include <qendian.h>

class XAudioResample{

private:

    SwrContext *swrContext = nullptr;
    int64_t max_dst_nb_samples;
    void init_out_frame(AVFrame *&out_frame, int64_t dst_nb_samples);
    QAudioFormat format;
    int target_sample_rate;
    AVSampleFormat target_sample_format;
    int64_t target_channel_layout;

public:
    // 将PCM数据重采样
    bool Open(QAudioFormat);
    void Close();
    int Resample(AVFrame *in_frame, char *out_data);
//    void decode_audio_resample(const char *media_path, const char *pcm_path);
    XAudioResample();
    ~XAudioResample();
};


#endif // XAUDIORESAMPLE_H
