﻿#include "XDemuxThread.h"
#include "XDemux.h"
#include "XVideoThread.h"
#include "XAudioThread.h"
#include <iostream>
#include <QDebug>
#include <QMediaDevices>
extern "C"
{
#include <libavformat/avformat.h>
}
#include "XDecode.h"
void XDemuxThread::SetPause(bool isPause)
{
    mux.lock();
    this->isPause = isPause;
    if (at) at->SetPause(isPause);
    if (vt) vt->SetPause(isPause);
   qDebug() << "XDemuxThread Pause"<<isPause;
    mux.unlock();
}
void XDemuxThread::SetVolume(double volume)
{

    if(!(volume>=0 || volume<=1)){
        qDebug()<<"XDemuxThread::SetVolume(double volume) illegal parameter";
        return;
    }
    mux.lock();
    if (at)
        at->SetVolume(volume);
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
    int re = 0;
    if (demux)
        re = demux->Seek(pos);
    if (re == 0)
        qDebug() << "demux->Seek failed!";
    AVRational time_base =  demux->getATimebase();
    long long seekPts = pos * demux->totalMs / 1000.0 / av_q2d(time_base);
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
    bool state = true;
    AVPacket *pkt = nullptr;
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
        if(state)
            pkt = demux->Read();
        if (!pkt)
        {
            mux.unlock();
            msleep(1);
            continue;
        }
        //判断数据是音频
        if (demux->IsAudio(pkt))
        {
            if(at)
                state = at->Push(pkt);
            mux.unlock();
        }
        else //视频
        {
            if (vt)
                state = vt->Push(pkt);
            mux.unlock();
            usleep(1);
        }
    }
    qDebug() << "videoTread exit";
}
//关闭线程清理资源
void XDemuxThread::Close()
{
    qDebug()<<"XDemuxThread::Close()";
    isExit = true;
    wait();
    if (vt) vt->Close();
    if (at) {
        qDebug()<<"at->Close()";
        at->Close();
    }
    if (syn) syn->clear();
    mux.lock();
    if(vt) {
        qDebug()<<"delete vt";
        delete vt;
    }
//    qDebug()<<"XDemuxThread::fff2";
    if(at) delete at;
//    if(syn) delete syn;
    vt = NULL;
    at = NULL;
    syn = NULL;
    mux.unlock();
}

void XDemuxThread::Clear()
{
    qDebug()<<"XDemuxThread::Clear()";
    mux.lock();
    if (demux) demux->Clear();
    if (vt) vt->Clear();
    if (at) at->Clear();
    if (syn) syn->clear();
    mux.unlock();
}

bool XDemuxThread::Open(const char *url, IVideoCall *call)
{
    qDebug()<<"XDemuxThread::Open"<<QString(url);
    if (url == 0 || url[0] == '\0')
        return false;

    mux.lock();
    if (!demux) demux = new XDemux();
    if (!vt){
        qDebug()<<"vt = new XVideoThread()";
        vt = new XVideoThread();
    }
    if (!at){
        qDebug()<<"at = new XAudioThread()";
        at = new XAudioThread();
    }
    if (!syn){
        qDebug()<<"syn = new SynModule()";
        syn = new SynModule();
    }
    //打开解封装
    bool re = demux->Open(url);
    if (!re)
    {
        std::cout << "demux->Open(url) failed!" << std::endl;
        return false;
    }
    syn->hasAudio =  demux->hasAudio();
    //打开视频解码器和处理线程
    if (!vt->Open(demux->CopyVPara(), call, demux->width, demux->height, syn))
    {
        re = false;
        std::cout << "vt->Open failed!" << std::endl;
    }
    //打开音频解码器和处理线程
    std::cout << "demux->sampleFormat : " << demux->sampleFormat << std::endl;
    if (!at->Open(demux->CopyAPara(), demux->sampleRate, demux->channels, syn))
    {
        re = false;
        std::cout << "at->Open failed!" << std::endl;
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

    AVRational time_base;
    time_base = demux->getATimebase();
    syn->setATimeBase(time_base);
    time_base = demux->getVTimebase();
    syn->setVTimeBase(time_base);

    mux.unlock();
    return re;
}
//启动所有线程
void XDemuxThread::Start()
{
    qDebug()<<"XDemuxThread::Start()";
    mux.lock();
    //启动当前线程
    if (!demux) demux = new XDemux();
    if (!vt) vt = new XVideoThread();
    if (!at) at = new XAudioThread();
    if (!syn) syn = new SynModule();
    QThread::start();
    if (vt)vt->start();
    if (at)at->start();
    if (syn) syn->start();
    mux.unlock();
    qDebug()<<"XDemuxThread::Start() finished";
}

XDemuxThread::XDemuxThread()
{
}

XDemuxThread::~XDemuxThread()
{
    qDebug() << "XDemuxThread::~XDemuxThread()";
    isExit = true;
    wait();
}

