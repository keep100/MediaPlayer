#ifndef XMEDIAMANAGER_H
#define XMEDIAMANAGER_H
#include <QThread>
#include <QImage>
#include <QDebug>
#include "XDemuxThread.h"
#include "XDemux.h"
#include "XDecode.h"
#include "IVideoCall.h"
#include <mutex>

class XDemux;
struct AVPacket;
struct AVFrame;
struct AVCodecParameters;
struct BriefInfo{
    QString mediaType = "";     //媒体类型 "video"/"audio"/""
    int totalMs = 0;            //总时长（毫秒）
    //视频信息
    int width = 0;
    int height = 0;
    //歌曲信息
    QString album = "";         //唱片集
    QString artist = "";        //参与创作的艺人
    QString title = "";         //标题
    //歌曲的封面或视频的缩略图
    QImage img;
};


class XMediaManager:public QObject{
    Q_OBJECT
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
    void end();
    void seek(double);
    States getCurState();
    void bind(QObject*);
protected:
    States  _curState;      //现态
    XDemuxThread *demuxThread = 0;

};

#endif // XMEDIAMANAGER_H
