#include "XDecode.h"

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
    qDebug()<<"XDecode::Close()";
    decode_mtx.lock();

    if (codec)
    {
        avcodec_free_context(&codec);
    }
    pts = 0;
    decode_mtx.unlock();
}

void XDecode::Clear()
{
    decode_mtx.lock();
    //清理解码缓冲
    if (codec)
    {
        avcodec_flush_buffers(codec);
    }

    decode_mtx.unlock();
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
        qDebug() << "can't find the codec id " << para->codec_id;
        return false;
    }

    decode_mtx.lock();
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
        decode_mtx.unlock();
        char buf[1024] = { 0 };
        av_strerror(re, buf, sizeof(buf) - 1);
        qDebug() << "avcodec_open2  failed! :" << buf;
        return false;
    }
    decode_mtx.unlock();
    return true;
}

bool XDecode::Send(AVPacket *pkt)
{
    //容错处理
    if (!pkt || pkt->size <= 0 || !pkt->data)
    {
        return false;
    }
    decode_mtx.lock();
    if (!codec)
    {
        decode_mtx.unlock();
        return false;
    }
    int re = avcodec_send_packet(codec, pkt);
    decode_mtx.unlock();
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
    decode_mtx.lock();
    if (!codec)
    {
        decode_mtx.unlock();
        return NULL;
    }
    AVFrame *frame = av_frame_alloc();
    int re = avcodec_receive_frame(codec, frame);
    decode_mtx.unlock();
    if (re != 0)
    {
        av_frame_free(&frame);
        return NULL;
    }
    pts = frame->pts;
    return frame;
}

AVSubtitle* XDecode::RecvSubtile(AVPacket *pkt) {
    //容错处理
    if (!pkt || pkt->size <= 0 || !pkt->data)
        return nullptr;
    int got_frame;
    decode_mtx.lock();
    if (!codec)
        return nullptr;
    AVSubtitle *subtitle = new AVSubtitle;
    int re = avcodec_decode_subtitle2(codec, subtitle, &got_frame, pkt);
    decode_mtx.unlock();
    av_packet_free(&pkt);
    if (re != 0 || got_frame <= 0)
        return nullptr;
    pts = subtitle->pts;
    return subtitle;
}

XDecode::XDecode()
{
}


XDecode::~XDecode()
{
}
