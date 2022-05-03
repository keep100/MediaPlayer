#include "XDemux.h"
#include <iostream>
using namespace std;
extern "C"
{
#include <libavformat/avformat.h>
}

static double r2d(AVRational r)
{
    //cout << r.num << " " << r.den << endl;
    return r.den == 0 ? 0 : r.num / (double)r.den;
}
XDemux::XDemux()
{

}
XDemux::~XDemux()
{

}
bool XDemux::IsAudio(AVPacket *pkt)
{
    if (!pkt) return false;
    if (pkt->stream_index == videoStream)
        return false;
    return true;
}
//清空读取缓存
void XDemux::Clear()
{
    mtx.lock();
    if (!ic)
    {
        mtx.unlock();
        return ;
    }
    //清理读取缓冲
    avformat_flush(ic);
    mtx.unlock();
}
void XDemux::Close()
{
    mtx.lock();
    if (!ic)
    {
        mtx.unlock();
        return;
    }
    avformat_close_input(&ic);
    //媒体总时长（毫秒）
    totalMs = 0;
    mtx.unlock();
}
bool XDemux::Seek(double pos)
{
    if (pos < 0.0 || pos > 1.0)
    {
        return false;
    }
    mtx.lock();
    if (!ic)
    {
        mtx.unlock();
        return false;
    }
    //清理读取缓冲
    avformat_flush(ic);

    long long seekPos = 0;
    seekPos = ic->streams[videoStream]->duration * pos;
    int re = av_seek_frame(ic, videoStream, seekPos, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
    mtx.unlock();
    if (re < 0) return false;
    return true;
}
//获取视频参数  返回的空间需要清理  avcodec_parameters_free
AVCodecParameters *XDemux::CopyVPara()
{
    mtx.lock();
    if (!ic)
    {
        mtx.unlock();
        return NULL;
    }
    AVCodecParameters *pa = avcodec_parameters_alloc();
    avcodec_parameters_copy(pa, ic->streams[videoStream]->codecpar);
    mtx.unlock();
    return pa;
}

//获取音频参数  返回的空间需要清理 avcodec_parameters_free
AVCodecParameters *XDemux::CopyAPara()
{
    mtx.lock();
    if (!ic)
    {
        mtx.unlock();
        return NULL;
    }
    AVCodecParameters *pa = avcodec_parameters_alloc();
    avcodec_parameters_copy(pa, ic->streams[audioStream]->codecpar);
    mtx.unlock();
    return pa;
}

bool XDemux::Open(const char *url)
{
    Close();
    //参数设置
    AVDictionary *opt = NULL;
    //尽晚加锁，尽早释放
    mtx.lock();
    int ret = avformat_open_input(&ic, url, NULL, &opt);
    if (ret)
    {
        mtx.unlock();
        char buf[1024]{0};
        av_strerror(ret, buf, 1024 - 1);
        cout << "open failed : " << buf << endl;
        getchar();
        return 0;
    }
    cout << "open succeeded ret = " << ret << endl;

    //获取流信息
    ret = avformat_find_stream_info(ic, NULL);
    if (ret)
    {
        cout << "get stream info failed\n";
        return 0;
    }
    //获取总时长 毫秒
    totalMs = ic->duration / (AV_TIME_BASE / 1000);
    cout << "total " << totalMs << endl;


    //获取视频详细信息
    //av_dump_format(ic, 0, url, 0);



    //获取音视频流信息
    //方法二
    videoStream = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    audioStream = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);

    //获取视频宽高
    width = ic->streams[videoStream]->codecpar->width;
    height = ic->streams[videoStream]->codecpar->height;

    //获取音频采样率和通道数
    sampleRate = ic->streams[audioStream]->codecpar->sample_rate;
    channels = ic->streams[audioStream]->codecpar->channels;
    sampleFormat = ic->streams[audioStream]->codecpar->format;
    mtx.unlock();
    return true;
}
//空间需要调用者释放 ，释放AVPacket对象空间，和数据空间 av_packet_free
AVPacket *XDemux::Read()
{
    mtx.lock();
    if (!ic)
    {
        mtx.unlock();
        return 0;
    }
    AVPacket *pkt = av_packet_alloc();
    //读取一帧，并分配空间
    int ret = av_read_frame(ic, pkt);
    if (ret != 0)
    {
        mtx.unlock();
        av_packet_free(&pkt);
        return 0;
    }
    //pts转换为毫秒
    pkt->pts = pkt->pts*(1000 * (r2d(ic->streams[pkt->stream_index]->time_base)));
    pkt->dts = pkt->dts*(1000 * (r2d(ic->streams[pkt->stream_index]->time_base)));
    mtx.unlock();
    //cout << pkt->pts << " "<<flush;
    return pkt;

}
