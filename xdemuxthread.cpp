#include "xdemuxthread.h"
#include "XDemux.h"
#include "XVideoThread.h"
#include "XAudioThread.h"
#include <iostream>
using namespace std;
void XDemuxThread::run()
{
    while (!isExit)
    {
        mux.lock();
        if (!demux)
        {
            mux.unlock();
            msleep(5);
            continue;
        }
        AVPacket *pkt = demux->Read();
        if (!pkt)
        {
            mux.unlock();
            msleep(5);
            continue;
        }
        //判断数据是音频
        if (demux->IsAudio(pkt))
        {
            if(at)at->Push(pkt);
        }
        else //视频
        {
            if (vt)vt->Push(pkt);
        }
        mux.unlock();
    }
}


bool XDemuxThread::Open(const char *url, IVideoCall *call)
{
    if (url == 0 || url[0] == '\0')
        return false;

    mux.lock();
    if (!demux) demux = new XDemux();
    if (!vt) vt = new XVideoThread();
    if (!at) at = new XAudioThread();

    //打开解封装
    bool re = demux->Open(url);
    if (!re)
    {
        cout << "demux->Open(url) failed!" << endl;
        return false;
    }
    //打开视频解码器和处理线程
    if (!vt->Open(demux->CopyVPara(), call, demux->width, demux->height))
    {
        re = false;
        cout << "vt->Open failed!" << endl;
    }
    //打开音频解码器和处理线程
    cout << "demux->sampleFormat : " << demux->sampleFormat << endl;
    if (!at->Open(demux->CopyAPara(), demux->sampleRate, demux->channels))
    {
        re = false;
        cout << "at->Open failed!" << endl;
    }
    mux.unlock();
    cout << "XDemuxThread::Open " << re << endl;
    return re;
}
//启动所有线程
void XDemuxThread::Start()
{
    mux.lock();
    //启动当前线程
    QThread::start();
    if (vt)vt->start();
    if (at)at->start();
    mux.unlock();
}
XDemuxThread::XDemuxThread()
{
}


XDemuxThread::~XDemuxThread()
{
    isExit = true;
    wait();
}

