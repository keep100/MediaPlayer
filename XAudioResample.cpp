#include "XAudioResample.h"
#include <QDebug>
#include <QAudioDevice>
#include <QMediaDevices>
extern "C" {
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>
}

XAudioResample::XAudioResample(){

}

void XAudioResample::init_out_frame(AVFrame *&out_frame, int64_t dst_nb_samples){

    av_frame_free(&out_frame);
    out_frame = av_frame_alloc();
    out_frame->sample_rate = target_sample_rate;
    out_frame->format = target_sample_format;
    out_frame->channel_layout = target_channel_layout;
    out_frame->nb_samples = dst_nb_samples;
    // 分配buffer
    av_frame_get_buffer(out_frame,0);
    av_frame_make_writable(out_frame);
}

bool XAudioResample::Open(QAudioFormat fmt){
    swrContext = nullptr;

    format = fmt;
    target_sample_rate = format.sampleRate();
    switch(format.sampleFormat()){
    case QAudioFormat::UInt8:{
        target_sample_format = AV_SAMPLE_FMT_U8P;break;
    }
    case QAudioFormat::Int16:{
        target_sample_format = AV_SAMPLE_FMT_S16P;break;
    }
    case QAudioFormat::Int32:{
        target_sample_format = AV_SAMPLE_FMT_S16P;break;
    }
    case QAudioFormat::Float:{
        target_sample_format = AV_SAMPLE_FMT_FLTP;break;
    }
    default:{
        target_sample_format = AV_SAMPLE_FMT_NONE;
    }
    }
    target_channel_layout = av_get_default_channel_layout(format.channelCount());
    qDebug()<<"target_sample_rate"<<target_sample_rate;
    qDebug()<<"target_sample_format"<<target_sample_format;
    qDebug()<<"target_channel_layout"<<target_channel_layout;
    qDebug()<<"XAudioResample::Open()";
    return true;
}

int XAudioResample::Resample(AVFrame *in_frame, char *out_data){
    if(swrContext==nullptr){
        qDebug()<<"swrContext==nullptr";
        qDebug()<<"in_frame->sample_rate"<<in_frame->sample_rate;
        qDebug()<<"in_frame->format"<<in_frame->format;
        qDebug()<<"in_frame->channel_layout"<<in_frame->channel_layout;
        qDebug()<<"target_sample_rate"<<target_sample_rate;
        qDebug()<<"target_sample_format"<<target_sample_format;
        qDebug()<<"target_channel_layout"<<target_channel_layout;
        swrContext = swr_alloc_set_opts(nullptr, target_channel_layout, static_cast<AVSampleFormat>(target_sample_format), target_sample_rate,
                                        in_frame->channel_layout, static_cast<AVSampleFormat>(in_frame->format),
                                        in_frame->sample_rate, 0, nullptr);
        qDebug()<<"ggggggggg";
        int re = swr_init(swrContext);
        if(re!=0){
            qDebug() << "swr_init  failed!";
            return -1;
        }
    }
    // 进行音频重采样
    int src_nb_sample = in_frame->nb_samples;
    // 为了保持从采样后 dst_nb_samples / dest_sample = src_nb_sample / src_sample_rate
    max_dst_nb_samples = av_rescale_rnd(src_nb_sample, target_sample_rate, in_frame->sample_rate, AV_ROUND_UP);
    // 从采样器中会缓存一部分，获取缓存的长度
    int64_t delay = swr_get_delay(swrContext, in_frame->sample_rate);
    int64_t dst_nb_samples = av_rescale_rnd(delay + in_frame->nb_samples, target_sample_rate, in_frame->sample_rate,
                                            AV_ROUND_UP);
    AVFrame *out_frame = nullptr;
    if(nullptr == out_frame){
        init_out_frame(out_frame, dst_nb_samples);
    }
    if (dst_nb_samples > max_dst_nb_samples) {
        // 需要重新分配buffer
//        qDebug() << "需要重新分配buffer" ;
        init_out_frame(out_frame, dst_nb_samples);
        max_dst_nb_samples = dst_nb_samples;
    }
    // 重采样
    int ret = swr_convert(swrContext, out_frame->data, dst_nb_samples,
                          const_cast<const uint8_t **>(in_frame->data), in_frame->nb_samples);
//    qDebug()<<"XAudioResample::Resample() ret:"<<ret;

    if(ret <= 0){
        qDebug() << "resample failed" ;
        return ret;
    } else{
        // 每帧音频数据量的大小
        int data_size = av_get_bytes_per_sample(static_cast<AVSampleFormat>(out_frame->format));
        int out_size = ret*out_frame->channels*data_size;
//        qDebug() << "resample succeed:" << ret << "----dst_nb_samples:" << dst_nb_samples  << "---data_size:" << data_size ;
        // 交错模式保持写入
        // 注意不要用 i < out_frame->nb_samples，因为重采样出来的点数不一定就是out_frame->nb_samples
        for (int i = 0; i < ret; i++) {
            for (int ch = 0; ch < out_frame->channels; ch++) {
                // 需要储存为pack模式
                memcpy_s(out_data + data_size * (i*out_frame->channels+ch), data_size, out_frame->data[ch] + data_size * i, data_size);
            }
        }
        return out_size;
    }

}

void XAudioResample::Close(){
    if(swrContext){
        swr_free(&swrContext);
    }
}


XAudioResample::~XAudioResample() {
    // todo 释放资源
}
