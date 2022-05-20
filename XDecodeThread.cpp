#include "XDecodeThread.h"
#include "XDecode.h"
void XDecodeThread::Close()
{
    Clear();

    //等待线程退出
    mux.lock();
    isExit = true;
    wait();
    decode->Close();


    delete decode;
    decode = NULL;
    mux.unlock();
}
void XDecodeThread::Clear()
{
    mux.lock();
    decode->Clear();
    while (!packs.empty())
    {
        AVPacket *pkt = packs.front();
        XFreePacket(&pkt);
        packs.pop_front();
    }
    mux.unlock();
}

int XDecodeThread::GetPacksNum()
{
    mux.lock();
    int ret = packs.size();
    mux.unlock();
    return ret;
}
//取出一帧数据，并出栈，如果没有返回NULL
AVPacket *XDecodeThread::Pop()
{
    mux.lock();
    if (packs.empty())
    {
        mux.unlock();
        return NULL;
    }
    AVPacket *pkt = packs.front();
    packs.pop_front();
    mux.unlock();
    return pkt;
}
void XDecodeThread::Push(AVPacket *pkt)
{
    if (!pkt)return;
    //阻塞
    while (!isExit)
    {
        mux.lock();
        if (packs.size() < maxList)
        {
            packs.push_back(pkt);
            mux.unlock();
            break;
        }
//        else
//        {
//            packs.pop_front();
//            packs.push_back(pkt);
//            mux.unlock();
//            break;
//        }
        mux.unlock();
        msleep(1);
    }
}


XDecodeThread::XDecodeThread()
{
    //打开解码器
    if (!decode) decode = new XDecode();
}


XDecodeThread::~XDecodeThread()
{	//等待线程退出
    isExit = true;
    wait();
}