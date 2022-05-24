#include <QThread>
#include <mutex>
#include <list>
struct AVCodecParameters;
#include "XDecodeThread.h"
#include "audioPlay2.h"
#include "SynModule.h"
#include "XAudioResample.h"
class XAudioThread:public XDecodeThread
{
public:
    //当前音频播放的pts
    long long pts = 0;
    //打开，不管成功与否都清理
    virtual bool Open(AVCodecParameters *para,int sampleRate,int channels, SynModule *syn);
    //停止线程，清理资源
    virtual void Close();
    virtual void Clear();
    void run();
    XAudioThread();
    virtual ~XAudioThread();
    void SetPause(bool isPause);
    void SetVolume(double volume);
    bool isPause = false;
    bool interupt = false;

protected:
    std::mutex amux;
//    XAudioPlay *ap = 0;
//    XResample *res = 0;
    audioPlay2 *ap2 = 0;
    XAudioResample *rsmp = 0;
    SynModule *syn = 0;
};
