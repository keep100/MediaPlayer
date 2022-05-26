#ifndef XDECODETHREAD_H
#define XDECODETHREAD_H

struct AVPacket;
class XDecode;
#include <list>
#include <mutex>
#include <QThread>
class XDecodeThread:public QThread
{
public:
    XDecodeThread();
    virtual ~XDecodeThread();
    virtual bool Push(AVPacket *pkt);
    virtual int GetPacksNum();

    //清理队列
    virtual void Clear();
    virtual void Close();

    //取出一帧数据，并出栈，如果没有返回NULL
    virtual AVPacket *Pop();
    //最大队列
    int maxList = 100;
    bool isExit = false;

protected:
    std::list <AVPacket *> packs;
    std::mutex mux;
    XDecode *decode = 0;

};


#endif // XDECODETHREAD_H
