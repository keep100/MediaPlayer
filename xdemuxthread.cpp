#include "XDemuxThread.h"
#include "XDemux.h"
#include "XVideoThread.h"
#include "XAudioThread.h"
#include <iostream>
#include <QDebug>
extern "C"
{
#include <libavformat/avformat.h>
}
#include "XDecode.h"
using namespace std;
void XDemuxThread::SetPause(bool isPause)
{
    mux.lock();
    cout << "XDemuxThread Pause\n";
    this->isPause = isPause;
    if (at) at->SetPause(isPause);
    if (vt) vt->SetPause(isPause);
    mux.unlock();
}
void XDemuxThread::Seek(double pos)
{
    Clear();
    mux.lock();
    bool status = this->isPause;
    mux.unlock();
    SetPause(true);
    mux.lock();
    if (demux)
        demux->Seek(pos);

    long long seekPts = pos*demux->totalMs;
    while(!isExit)
    {
        AVPacket *pkt = demux->ReadVideo();
        if (!pkt) break;
        if (vt->RepaintPts(seekPts, pkt))
        {
            this->pts = seekPts;
            break;
        }
    }

    mux.unlock();
    if(!status)
        SetPause(false);
}
void XDemuxThread::run()
{
    while (!isExit)
    {
        mux.lock();
        if (isPause)
        {
            mux.unlock();
            msleep(5);
            continue;
        }
        if (!demux)
        {
            mux.unlock();
            msleep(1);
            continue;
        }
        //音视频同步
        if (vt && at)
        {
            pts = at->pts;
            vt->synpts = at->pts;
        }
        AVPacket *pkt = demux->Read();
        if (!pkt)
        {
            mux.unlock();
            msleep(1);
            continue;
        }
        //判断数据是音频
        if (demux->IsAudio(pkt))
        {
            if(at)at->Push(pkt);
            mux.unlock();
            //msleep(1);
        }
        else //视频
        {

            if (vt)vt->Push(pkt);
            mux.unlock();
            msleep(1);
        }


    }
}
//关闭线程清理资源
void XDemuxThread::Close()
{
    isExit = true;
    wait();
    if (vt) vt->Close();
    if (at) at->Close();
    mux.lock();
    delete vt;
    delete at;
    vt = NULL;
    at = NULL;
    mux.unlock();
}
void XDemuxThread::Clear()
{
    mux.lock();
    if (demux) demux->Clear();
    if (vt) vt->Clear();
    if (at) at->Clear();
    mux.unlock();
}

bool XDemuxThread::Open(const char *url, IVideoCall *call)
{
    if (url == 0 || url[0] == '\0')
        return false;

    mux.lock();

    if (!demux) demux = new XDemux();
    if (!vt) vt = new XVideoThread();
    if (!at) at = new XAudioThread();

    cout << "vt->GetPacksNum() : " << vt->GetPacksNum() << " at->GetPacksNum() : " << at->GetPacksNum() << endl;
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
    if (isFirst)
    {
        at->interupt = 1;
    }
    if (!isFirst)
    {
        isFirst = 1;
    }
    totalMs = demux->totalMs;
    mux.unlock();
    cout << "XDemuxThread::Open " << re << endl;
    return re;
}
//启动所有线程
void XDemuxThread::Start()
{
    mux.lock();
    //启动当前线程
    if (!demux) demux = new XDemux();
    if (!vt) vt = new XVideoThread();
    if (!at) at = new XAudioThread();
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
    cout << "XDemuxThread::~XDemuxThread()\n";
    isExit = true;
    wait();
}

