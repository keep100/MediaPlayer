#pragma once
///解码和显示视频
struct AVPacket;
struct AVCodecParameters;
class XDecode;
class SynModule;
#include <list>
#include <mutex>
#include <QThread>
#include "IVideoCall.h"
#include "XDecodeThread.h"
#include "XSubtitleThread.h"
#include "util/imageprovider.h"
#include "util/bufferqueue.h"
#include "util/yuvdata.h"

class XVideoThread: public XDecodeThread
{
    friend class XSubtitleThread;
public:
    //打开，不管成功与否都清理
    virtual bool Open(AVCodecParameters *para, IVideoCall *call, int width, int height, SynModule *syn);
    void run();

    XVideoThread();
    virtual ~XVideoThread();
    //同步时间，由外部传入
    long long synpts = 0;

    void SetPause(bool isPause);
    bool isPause = false;
    virtual bool RepaintPts(long long seekPts, AVPacket *pkt);
    ShowImage *showImage;
    QImage frameToImage(AVFrame *frame);
    QString fileName;
    AVRational timebase;

private:
    std::mutex vmux;
    IVideoCall *call = 0;
    AVCodecParameters *codecParam;
    SynModule *syn = 0;

    bool subtitleOpened = false;
    AVFilterContext *buffersrcContext = nullptr;
    AVFilterContext *buffersinkContext = nullptr;
    // 将视频帧转成YUV420P格式
    std::shared_ptr<YUVData> convertToYUV420P(AVFrame *videoFrame);

};
