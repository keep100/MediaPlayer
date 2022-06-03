#pragma once
#ifndef XDECODE_H
#define XDECODE_H

struct AVCodecParameters;
struct AVCodecContext;
struct AVFrame;
struct AVPacket;
#include <mutex>
extern void XFreePacket(AVPacket **pkt);
extern void XFreeFrame(AVFrame **frame);
class XDecode
{
public:
    long long pts = 0;
    bool isAudio = false;
    AVCodecContext *codec = 0;
    //打开解码器,不管成功与否都释放para空间
    virtual bool Open(AVCodecParameters *para);

    //发送到解码线程，不管成功与否都释放pkt空间（对象和媒体内容）
    virtual bool Send(AVPacket *pkt);

    //获取解码数据，一次send可能需要多次Recv，获取缓冲中的数据Send NULL在Recv多次
    //每次复制一份，由调用者释放 av_frame_free
    virtual AVFrame* Recv();
    AVSubtitle* RecvSubtile(AVPacket *pkt);

    virtual void Close();
    virtual void Clear();
    AVRational getTimebase()const {
        AVRational res;
        if (codec)
            return codec->time_base;
        else
            return res;
    }

    XDecode();
    virtual ~XDecode();
protected:

    std::mutex decode_mtx;
};

#endif // XDECODE_H
