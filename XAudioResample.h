#ifndef XAUDIORESAMPLE_H
#define XAUDIORESAMPLE_H

extern "C" {
#include "libavformat/avformat.h"
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>
#include <libavutil/frame.h>
#include <libavutil/opt.h>
#include <libavutil/channel_layout.h>
}

#ifndef AUDIO_TARGET_SAMPLE
#define AUDIO_TARGET_SAMPLE 48000
#endif

class XAudioResample{

private:
//    AVFormatContext *avFormatContext = nullptr;
//    AVCodecContext *avCodecContext = nullptr;
//    AVPacket *avPacket = nullptr;
//    AVFrame *avFrame = nullptr;
//    FILE *pcm_out = nullptr;
    SwrContext *swrContext = nullptr;
//    AVFrame *out_frame = nullptr;
    int64_t max_dst_nb_samples;

//    void resample();
    void initOutFrame(AVFrame *out_frame, int64_t dst_nb_samples);

public:
    // 将PCM数据重采样
    bool Open(AVCodecParameters *para, bool isClearPara);
    void Close();
    int Resample(AVFrame *in_frame, unsigned char *out_data);
//    void decode_audio_resample(const char *media_path, const char *pcm_path);
    ~XAudioResample();
};


#endif // XAUDIORESAMPLE_H
