#include "XAudioThread.h"
#include "XDecode.h"
#include "XAudioPlay.h"
#include "XResample.h"
#include <iostream>
#include "XAudioResample.h"
using namespace std;
//停止线程，清理资源
void XAudioThread::Close()
{
    XDecodeThread::Close();
    if (res)
    {
        res->Close();
        amux.lock();
        delete res;
        res = NULL;
        amux.unlock();
    }
    if (ap)
    {
        ap->Close();
        amux.lock();
        ap = NULL;
        amux.unlock();
    }
}
void XAudioThread::Clear()
{
    XDecodeThread::Clear();
    mux.lock();
    if (ap)
    {
        ap->Clear();
    }
    mux.unlock();
}
bool XAudioThread::Open(AVCodecParameters *para,int sampleRate, int channels)
{
    if (!para)return false;
    Clear();

    amux.lock();
    pts = 0;
    bool re = true;
    if (!res->Open(para, false))
    {
        cout << "XResample open failed!" << endl;
        re = false;
    }
    ap->sampleRate = sampleRate;
    ap->channels = channels;
    if (!ap->Open())
    {
        re = false;
        cout << "XAudioPlay open failed!" << endl;
    }
    if (!decode->Open(para))
    {
        cout << "audio XDecode open failed!" << endl;
        re = false;
    }
    amux.unlock();
    cout << "XAudioThread::Open :" << re << endl;
    return re;
}
void XAudioThread::SetPause(bool isPause)
{
    //amux.lock();
    cout << "XAudioThread Pause\n";
    this->isPause = isPause;
    if (ap)
        ap->SetPause(isPause);
    //amux.unlock();
}

void XAudioThread::SetVolume(double volume)
{
    //amux.lock();
    cout << "XAudioThread SetVolume\n";
    if (ap)
        ap->SetVolume(volume);
    //amux.unlock();
}


void XAudioThread::run()
{
    unsigned char *pcm = new unsigned char[1024 * 1024];
    while (!isExit)
    {
        amux.lock();

        if (isPause)
        {
            amux.unlock();
            msleep(5);
            continue;
        }
        //没有数据
        //if (packs.empty() || !decode || !res || !ap)
        //{
        //	mux.unlock();
        //	msleep(1);
        //	continue;
        //}

        //AVPacket *pkt = packs.front();
        //packs.pop_front();
        AVPacket *pkt = Pop();
        bool re = decode->Send(pkt);
        if (!re)
        {
            amux.unlock();
            msleep(1);
            continue;
        }

        //一次send 多次recv
        cout << "Audio...\n";
        while (!isExit)
        {
            if (isPause)
            {
                break;
            }
            AVFrame * frame = decode->Recv();
            if (!frame) break;

            //减去缓冲中未播放的时间
            cout << "ap->GetNoPlayMs() " << ap->GetNoPlayMs() << endl;
            pts = decode->pts - ap->GetNoPlayMs();


            //cout << "audio pts = " << pts << endl;

            //重采样
            int size = res->Resample(frame, pcm);
            //播放音频
            while (!isExit)
            {

                if (size <= 0)break;
                //缓冲未播完，空间不够
                if (ap->GetFree() < size || isPause)
                {
                    msleep(1);
                    continue;
                }
                ap->Write(pcm, size);
                break;
            }
        }
        amux.unlock();
    }
    cout << "audioTread exit \n";
    delete[] pcm;
}

XAudioThread::XAudioThread()
{
    if (!res) res = new XResample();
    if (!ap) ap = XAudioPlay::Get();
}


XAudioThread::~XAudioThread()
{
    //等待线程退出
    cout << "XAudioThread::~XAudioThread()\n";
    isExit = true;
    wait();
}
