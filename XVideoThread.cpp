#include "XVideoThread.h"
#include "XDecode.h"
#include <iostream>
using namespace std;
//打开，不管成功与否都清理
bool XVideoThread::Open(AVCodecParameters *para, IVideoCall *call,int width,int height)
{
    if (!para)return false;

    Clear();

    vmux.lock();
    synpts = 0;
    //初始化显示窗口
    this->call = call;
    if (call)
    {
        call->Init(width, height);
    }
    vmux.unlock();
    int re = true;
    if (!decode->Open(para))
    {
        cout << "video XDecode open failed!" << endl;
        re = false;
    }

    cout << "XVideoThread::Open :" << re << endl;
    return re;
}
void XVideoThread::SetPause(bool isPause)
{
    vmux.lock();
    this->isPause = isPause;
    vmux.unlock();
}
void XVideoThread::run()
{
    while (!isExit)
    {
        vmux.lock();
        //cout << "synpts = " << synpts << " dpts =" << decode->pts << endl;
        if (this->isPause)
        {
            vmux.unlock();
            msleep(5);
            continue;
        }
        cout << "Video...\n";
        //音视频同步
        if (synpts >0 && synpts < decode->pts)
        {
            vmux.unlock();
            cout << "syn\n";
            msleep(1);
            continue;
        }
        AVPacket *pkt = Pop();
        bool re = decode->Send(pkt);
        if (!re)
        {
            vmux.unlock();
            msleep(1);
            continue;
        }
        //一次send 多次recv
        while (!isExit)
        {
            AVFrame * frame = decode->Recv();
            if (!frame) break;
            //显示视频
            if (call)
            {
                call->Repaint(frame);
            }
            msleep(1);

        }
        vmux.unlock();
    }
    cout << "videoTread exit \n";
}
bool XVideoThread::RepaintPts(long long seekPts, AVPacket *pkt)
{
    vmux.lock();
    bool ret = decode->Send(pkt);
    if (!ret)
    {
        vmux.unlock();
        return 1; //表示结束解码
    }
    AVFrame *frame = decode->Recv();
    if (!frame)
    {
        vmux.unlock();
        return 0;
    }
    //到达位置
    if (decode->pts >= seekPts)
    {
        if (call)
            call->Repaint(frame);
        vmux.unlock();
        return 1;
    }
    XFreeFrame(&frame);
    vmux.unlock();
    return false;
}
XVideoThread::XVideoThread()
{
}


XVideoThread::~XVideoThread()
{
    cout << "XVideoThread::~XVideoThread()\n";
    isExit = true;
    wait();
}
