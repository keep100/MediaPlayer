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
#include "until/yuvdata.h"

class XDemux;
struct AVPacket;
struct AVFrame;
struct AVCodecParameters;
struct BriefInfo{
    QString mediaType = "";     //媒体类型 （"video", "audio", ""）
    QString codecId = "";       //编码格式
    int totalMs = 0;            //总时长（ms）
    int bitRate = 0;            //比特率（bps）
    int channels = 0;           //声道数
    QImage img;                 //歌曲的封面或视频的缩略图
    //视频特有信息
    int width = 0;
    int height = 0;
    //歌曲特有信息
    QString album = "";         //唱片集
    QString artist = "";        //参与创作的艺人
    QString title = "";         //标题

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
    //~XMediaManager();
    void playMedia(QString url);
    bool open(const char *url);
    void play();
    void pause();
    void setVolume(float v);
    void end();
    void seek(double);
    States getCurState();

    void bind(QObject*);
signals:
   void transmit_yuv(std::shared_ptr<YUVData> t, int64_t time);

protected:
    States  _curState;      //现态
    XDemuxThread *demuxThread = 0;

};

#endif // XMEDIAMANAGER_H
