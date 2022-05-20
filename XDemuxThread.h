#ifndef XDEMUXTHREAD_H
#define XDEMUXTHREAD_H


#include <QThread>
#include "IVideoCall.h"
#include <mutex>
class XDemux;
class XVideoThread;
class XAudioThread;
class XDemuxThread:public QThread
{
public:
    //创建对象并打开
    virtual bool Open(const char *url, IVideoCall *call);

    //启动所有线程
    virtual void Start();
    //关闭线程清理资源
    virtual void Close();
    virtual void Clear();
    //跳转
    virtual void Seek(double);

    void run();
    XDemuxThread();
    virtual ~XDemuxThread();
    bool isExit = false;
    long long pts = 0;
    long long totalMs = 0;
    void SetPause(bool isPause);
    void SetVolume(double volume);
    bool isPause = false;
    bool isFirst = false;
protected:
    std::mutex mux;
    XDemux *demux = 0;
    XVideoThread *vt = 0;
    XAudioThread *at = 0;
};

#endif // XDEMUXTHREAD_H