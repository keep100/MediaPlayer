#ifndef XDEMUX_H
#define XDEMUX_H

struct AVFormatContext;
struct AVPacket;
struct AVCodecParameters;
#include <mutex>
class XDemux
{
public:
    XDemux();
    virtual ~XDemux();
    virtual bool Open(const char *url);
    virtual AVPacket *Read();

    virtual bool IsAudio(AVPacket *pkt);
    //获取视频参数  返回的空间需要清理  avcodec_parameters_free
    AVCodecParameters *CopyVPara();

    //获取音频参数  返回的空间需要清理 avcodec_parameters_free
    AVCodecParameters *CopyAPara();

    //seek 位置 pos 0.0 ~1.0
    virtual bool Seek(double pos);

    //清空读取缓存
    virtual void Clear();
    virtual void Close();

    virtual AVPacket *ReadVideo();


    //总时长 毫秒
    int totalMs = 0;
    int width = 0;
    int height = 0;
    //音频信息
    int sampleRate = 0;
    int channels = 0;
    int sampleFormat = 2;


protected:
    std::mutex mtx;

    AVFormatContext *ic = NULL;
    int videoStream = -1;
    int audioStream = -1;

};

#endif // XDEMUX_H