#include "XAudioThread.h"
#include "XDecode.h"
#include "util/pcmdata.h"
#include <iostream>
using namespace std;
//停止线程，清理资源
void XAudioThread::Close()
{
    XDecodeThread::Close();

    if(rsmp){
        rsmp->Close();
        delete rsmp;
        rsmp = nullptr;
    }
    if(ap2){
        ap2->Close();
        ap2 = nullptr;
    }
}
void XAudioThread::Clear()
{
    XDecodeThread::Clear();

    if(ap2){
        ap2->Clear();
    }
}
bool XAudioThread::Open(AVCodecParameters *para,int sampleRate, int channels, SynModule *syn)
{
    if (!para)return false;
    Clear();

    amux.lock();
    pts = 0;
    bool re = true;

    this->syn = syn;

    QMediaDevices media(this);
    QAudioDevice deviceInfo = media.defaultAudioOutput();
    if(!ap2->Open(deviceInfo)){
        re = false;
        qDebug()<<"!ap2->Open()";
    }
    if(!rsmp->Open(ap2->GetFormat())){
        re = false;
        qDebug()<<"!rsmp->Open(ap2->GetFormat())";
    }

    if (!decode->Open(para))
    {
        re = false;
        qDebug()<<"audio XDecode open failed!";
    }
    amux.unlock();
    qDebug() << "XAudioThread::Open :" << re ;
    return re;
}
void XAudioThread::SetPause(bool isPause)
{

    this->isPause = isPause;
    //    if (ap)
    //        ap->SetPause(isPause);
    if(ap2) ap2->SetPause(isPause);

}

void XAudioThread::SetVolume(double volume)
{
    //amux.lock();
    qDebug() << "XAudioThread SetVolume"<<volume;
    if (ap2)
        ap2->SetVolume(volume);
    //amux.unlock();
}


void XAudioThread::run()
{
    //    unsigned char *pcm = new unsigned char[1024 * 1024];
    char resample_data[1024*256];
    while (!isExit)
    {
        amux.lock();

        if (isPause)
        {
            amux.unlock();
            msleep(5);
            continue;
        }

        AVPacket *pkt = Pop();

        bool re = decode->Send(pkt);
        if (!re)
        {
            amux.unlock();
            msleep(1);
            continue;
        }

        //一次send 多次recv
        while (!isExit)
        {
            if (isPause)
            {
                break;
            }
            int64_t pts;
            AVFrame * frame = decode->Recv();
            if (!frame) break;

            int size = rsmp->Resample(frame, resample_data);
            if ((pts = frame->pts) == AV_NOPTS_VALUE)
                pts = 0;
            while (ap2->GetFree() < size)
                msleep(5);
            syn->setAClock(pts, ap2->GetNoPlayMs());
            ap2->Write(resample_data, size);
        }
        amux.unlock();
    }
    qDebug() << "audioTread exit \n";
}

XAudioThread::XAudioThread()
{
    //    if (!res) res = new XResample();
    //    if (!ap) ap = XAudioPlay::Get();
    if(!ap2) ap2 = new audioPlay2();
    if(!rsmp) rsmp = new XAudioResample();

}


XAudioThread::~XAudioThread()
{
    //等待线程退出
    qDebug() << "XAudioThread::~XAudioThread()\n";
    isExit = true;
    wait();
}
