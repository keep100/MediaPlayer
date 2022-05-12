#ifndef XMEDIAMANAGER_H
#define XMEDIAMANAGER_H
#include <QThread>
#include <QImage>
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

class XMediaManager{
public:
    static BriefInfo getBriefInfo(const char *url);
    static QImage getQImageFromFrame(const AVFrame* frame, AVCodecParameters *codecParam);
protected:

};

#endif // XMEDIAMANAGER_H
