#include "XAudioThread.h"
#include "XDecode.h"

#include <iostream>
using namespace std;
//停止线程，清理资源
void XAudioThread::Close()
{
    XDecodeThread::Close();
//    if (res)
//    {
//        res->Close();
//        amux.lock();
//        delete res;
//        res = NULL;
//        amux.unlock();
//    }
//    if (ap)
//    {
//        ap->Close();
//        amux.lock();
//        ap = NULL;
//        amux.unlock();
//    }
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
//    mux.lock();
//    if (ap)
//    {
//        ap->Clear();
//    }
//    mux.unlock();
    if(ap2){
        ap2->Clear();
    }
}
bool XAudioThread::Open(AVCodecParameters *para,int sampleRate, int channels)
{
    if (!para)return false;
    Clear();

    amux.lock();
    pts = 0;
    bool re = true;

//    if (!res->Open(para, false))
//    {
//        qDebug() << "XResample open failed!" ;
//        re = false;
//    }
//    ap->sampleRate = sampleRate;
//    ap->channels = channels;
//    if (!ap->Open())
//    {
//        re = false;
//        qDebug() << "XAudioPlay open failed!" ;
//    }

    if(!ap2->Open()){
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
    qDebug() << "XAudioThread Pause\n";
    this->isPause = isPause;
//    if (ap)
//        ap->SetPause(isPause);
    if(ap2) ap2->SetPause(isPause);

}

void XAudioThread::SetVolume(double volume)
{
    //amux.lock();
    cout << "XAudioThread SetVolume\n";
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
        qDebug() << "Audio...\n";
        while (!isExit)
        {
            if (isPause)
            {
                break;
            }
            AVFrame * frame = decode->Recv();
            if (!frame) break;

//            //减去缓冲中未播放的时间
//            qDebug() << "ap->GetNoPlayMs() " << ap->GetNoPlayMs() ;
//            pts = decode->pts - ap->GetNoPlayMs();
//            //重采样
//            int size = res->Resample(frame, pcm);
//            //播放音频
//            while (!isExit)
//            {

//                if (size <= 0)break;
//                //缓冲未播完，空间不够
//                if (ap->GetFree() < size || isPause)
//                {
//                    msleep(1);
//                    continue;
//                }
//                ap->Write(pcm, size);
//                break;
//            }

            pts = decode->pts - ap2->GetNoPlayMs();
            int size = rsmp->Resample(frame, resample_data);
            while(!isExit){
                if(size<=0)break;
                if(ap2->GetFree() < size || isPause){
                    msleep(5);
                    continue;
                }
                ap2->Write(resample_data, size);
                break;
            }

        }
        amux.unlock();
    }
    qDebug() << "audioTread exit \n";
//    delete[] pcm;
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
