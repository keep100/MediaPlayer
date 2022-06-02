#pragma once
#ifndef XSUBTILETHREAD_H
#define XSUBTILETHREAD_H

#include "XDecodeThread.h"
class XVideoThread;
class XSubtitleThread : public XDecodeThread
{
public:
    ~XSubtitleThread(){avcodec_parameters_free(&codecParam);}
    //打开，不管成功与否都清理
    bool Open(AVCodecParameters *para);
    // 初始化字幕过滤器
    bool init_subtitle_filter(AVFilterContext *&buffersrcContext,
                              AVFilterContext *&buffersinkContext,
                              QString args, QString filterDesc);
    void setVideoThread(XVideoThread *vt) {
        this->vt = vt;
    }
private:
    std::mutex vmux;
    AVCodecParameters *codecParam;
    XVideoThread *vt;
};

#endif // XSUBTILETHREAD_H
