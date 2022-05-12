#include "XMediaManager.h"
extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
#include "libavformat/avio.h"
#include "libavutil/imgutils.h"
}
#include <iostream>
#include <QThread>
#include <QDebug>
using namespace std;
BriefInfo XMediaManager::getBriefInfo(const char * url){
    BriefInfo briefInfo;
    if (url == 0 || url[0] == '\0')
        return briefInfo;
    XDemux *demux = new XDemux();
    XDecode *decode = new XDecode();
    demux->Open(url);
    briefInfo.totalMs = demux->totalMs;
    briefInfo.width = demux->width;
    briefInfo.height = demux->height;
    briefInfo.channels = demux->channels;
    briefInfo.sampleRate = demux->sampleRate;
    if (demux->CopyVPara())
        briefInfo.mediaType = "video" ;
    else briefInfo.mediaType = "audio" ;

    qDebug()<<briefInfo.mediaType;

    if(decode->Open(demux->CopyVPara())){
        AVFrame *pFrame = av_frame_alloc();
        //先recv，后send
        int numFrame = 0;
        while (true)
        {
            pFrame = decode->Recv();
            if (pFrame)
            {
                numFrame++;
    //            qDebug() << "receive--------------------------";
                if(numFrame==1000)break;
            }
    //        qDebug() << "send--------------------------";
            AVPacket *pkt = demux->ReadVideo();
            qDebug()<<decode->Send(pkt);
    //        _sleep(1);
        }
        briefInfo.qimage = getQImageFromFrame(pFrame, demux->CopyVPara());
    }
    return briefInfo;
}

QImage XMediaManager::getQImageFromFrame(const AVFrame* frame, AVCodecParameters *para)
{

    qDebug() << "frameToImage";
    QImage output(frame->width, frame->height, QImage::Format_RGB32); //构造一个QImage用作输出
    qDebug()<<frame->width<<", "<<frame->height;
    qDebug() << "frameToImage2";
    int outputLineSize[4];                                                       //构造AVFrame到QImage所需要的数据
    av_image_fill_linesizes(outputLineSize, AV_PIX_FMT_RGB32, para->width);
    uint8_t *outputDst[] = {output.bits()};
    //构造一个格式转换上下文
    SwsContext *imgConvertContext = sws_getContext(
                                        para->width, para->height,
                                        (AVPixelFormat)para->format, para->width, para->height,
                                        AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);

    //把解码得到的损坏的像素数据剔除
    sws_scale(imgConvertContext, frame->data, frame->linesize, 0, para->height, outputDst, outputLineSize);
    output.save("C:\\Users\\16409\\Desktop\\oneFrame.jpg");
    sws_freeContext(imgConvertContext);
//    av_frame_free(&frame);
    return output;

}
