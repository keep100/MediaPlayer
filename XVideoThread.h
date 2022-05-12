///解码和显示视频
struct AVPacket;
struct AVCodecParameters;
class XDecode;
#include <list>
#include <mutex>
#include <QThread>
#include "IVideoCall.h"
#include "XDecodeThread.h"
#include "imageprovider.h"

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

protected:
    std::mutex vmux;
    IVideoCall *call = 0;
    AVCodecParameters *codecParam;


};
