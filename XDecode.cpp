#include "XDecode.h"
extern "C"
{
#include<libavcodec/avcodec.h>
}
#include <iostream>
using namespace std;
void XFreePacket(AVPacket **pkt)
{
    if (!pkt || !(*pkt))
    {
        return;
    }
    av_packet_free(pkt);
}
void XFreeFrame(AVFrame **frame)
{
    if (!frame || !(*frame))
    {
        return;
    }
    av_frame_free(frame);
}
void XDecode::Close()
{
    mtx.lock();
    if (codec)
    {
        avcodec_close(codec);
        avcodec_free_context(&codec);
    }
    pts = 0;
    mtx.unlock();
}

void XDecode::Clear()
{
    mtx.lock();
    //清理解码缓冲
    if (codec)
    {
        avcodec_flush_buffers(codec);
    }

    mtx.unlock();
}

//打开解码器
bool XDecode::Open(AVCodecParameters *para)
{
    if (!para)
    {
        return false;
    }
    Close();
    //解码器打开
    //找到解码器
    const AVCodec *vcodec = avcodec_find_decoder(para->codec_id);
    if (!vcodec)
    {
        avcodec_parameters_free(&para);
        cout << "can't find the codec id " << para->codec_id << endl;
        return false;
    }
    //cout << "find the AVCodec " << para->codec_id << endl;

    mtx.lock();
    codec = avcodec_alloc_context3(vcodec);

    ///配置解码器上下文参数
    avcodec_parameters_to_context(codec, para);
    avcodec_parameters_free(&para);

    //八线程解码
    codec->thread_count = 8;

    ///打开解码器上下文
    int re = avcodec_open2(codec, 0, 0);
    if (re != 0)
    {
        avcodec_free_context(&codec);
        mtx.unlock();
        char buf[1024] = { 0 };
        av_strerror(re, buf, sizeof(buf) - 1);
        cout << "avcodec_open2  failed! :" << buf << endl;
        return false;
    }
    mtx.unlock();
    //cout << " avcodec_open2 success!" << endl;
    return true;
}
bool XDecode::Send(AVPacket *pkt)
{
    //容错处理
    if (!pkt || pkt->size <= 0 || !pkt->data)
    {
        return false;
    }
    mtx.lock();
    if (!codec)
    {
        mtx.unlock();
        return false;
    }
    int re = avcodec_send_packet(codec, pkt);
    mtx.unlock();
    av_packet_free(&pkt);
    if (re != 0)
    {
        return false;
    }
    return true;
}

//获取解码数据，一次send可能需要多次Recv，获取缓冲中的数据Send NULL在Recv多次
//每次复制一份，由调用者释放 av_frame_free
AVFrame *XDecode::Recv()
{
    mtx.lock();
    if (!codec)
    {
        mtx.unlock();
        return NULL;
    }
    AVFrame *frame = av_frame_alloc();
    int re = avcodec_receive_frame(codec, frame);
    mtx.unlock();
    if (re != 0)
    {
        av_frame_free(&frame);
        return NULL;
    }
    pts = frame->pts;
    //cout << "["<<frame->linesize[0] << "] " << flush;
    return frame;
}
XDecode::XDecode()
{
}


XDecode::~XDecode()
{
}
