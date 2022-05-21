///解码和显示视频
struct AVPacket;
struct AVCodecParameters;
class XDecode;

#include <list>
#include <mutex>
#include <QThread>
#include "IVideoCall.h"
#include "XDecodeThread.h"
#include "until/imageprovider.h"
#include "until/bufferqueue.h"
#include "until/yuvdata.h"

class XVideoThread: public XDecodeThread
{
public:
    //打开，不管成功与否都清理
    virtual bool Open(AVCodecParameters *para, IVideoCall *call, int width, int height);
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
    std::shared_ptr<YUVData> getYUVData();


protected:
    std::mutex vmux;
    IVideoCall *call = 0;
    AVCodecParameters *codecParam;
    // yuv视频帧缓冲区
    BufferQueue<std::shared_ptr<YUVData>> yuvQueue;

    // 将视频帧转成YUV420P格式
    std::shared_ptr<YUVData> convertToYUV420P(AVFrame *videoFrame);

};
