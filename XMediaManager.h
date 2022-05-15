#ifndef XMEDIAMANAGER_H
#define XMEDIAMANAGER_H
#include <QThread>
#include <QImage>
#include <QDebug>
#include "xdemuxthread.h"
#include "XDemux.h"
#include "XDecode.h"
#include "IVideoCall.h"
#include <mutex>
class XDemux;
struct AVPacket;
struct AVFrame;
struct AVCodecParameters;
struct BriefInfo{
    QString mediaType = "";
    //总时长 毫秒
    int totalMs = 0;
    int width = 0;
    int height = 0;
    //音频信息
    int sampleRate = 0;
    int channels = 0;
    QImage qimage;
};

////FSM状态项
//class FSMItem{
//public:
//    //枚举所有状态
//    enum States{ INITIAL=0, READY, PLAYING, PAUSED, END};
//    //枚举所有事件
//    enum Events{EVENT0 = 0, EVENT1, EVENT2};

//private:
//    States  _curState;      //现态
//    Events  _event;         //条件
//    void    (*_action)();   //动作
//    States  _nextState;     //次态
//    QStringList sl = {"INITIAL", "READY", "PLAYING", "PAUSED", "END"};

//public:
//    //初始化构造函数
//    FSMItem(States curState, Events event, void(*action)(), States nextState)
//        :_curState(curState), _event(event), _action(action), _nextState(nextState) {}

//private:
//    //动作函数
//    void showCurState()
//    {
//        qDebug()<<"current state: "<<sl[_curState];
//    }
//};


class XMediaManager{
private:
//    FSMItem fsmItem;
public:
    static BriefInfo getBriefInfo(const char *);
    static QImage getQImageFromFrame(const AVFrame* frame, AVCodecParameters *codecParam);

    enum States{ INITIAL=0, READY, PLAYING, PAUSED, END};

    XMediaManager();
    ~XMediaManager();
    bool open(const char *url);
    void play();
    void pause();
    void terminate();
    void seek(double);
    States getCurState();
protected:
    States  _curState;      //现态
    XDemuxThread *demuxThread = 0;

};

#endif // XMEDIAMANAGER_H
