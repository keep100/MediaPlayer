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
    AVPacket *pkt = demux->Read();
    if (demux->IsAudio(pkt))
        briefInfo.mediaType = "audio" ;
    else briefInfo.mediaType = "video" ;
    decode->Open(demux->CopyVPara());
    decode->Send(pkt);
    AVFrame *pFrame = av_frame_alloc();
    pFrame = decode->Recv();
//    qDebug()<<pFrame->width<<", "<<pFrame->height;
    briefInfo.qimage = getQImageFromFrame(pFrame, demux->CopyVPara());
    qDebug()<<"ok";
    return briefInfo;
}

QImage XMediaManager::getQImageFromFrame(const AVFrame* frame, AVCodecParameters *codecParam)
{

    qDebug() << "frameToImage";
    QImage output(frame->width, frame->height, QImage::Format_RGB32); //构造一个QImage用作输出
//    int outputLineSize[4];                                                       //构造AVFrame到QImage所需要的数据
//    av_image_fill_linesizes(outputLineSize, AV_PIX_FMT_RGB32, frame->width);
//    uint8_t *outputDst[] = {output.bits()};
//    //构造一个格式转换上下文
//    SwsContext *imgConvertContext = sws_getContext(
//                                        codecParam->width, codecParam->height,
//                                        (AVPixelFormat)codecParam->format, codecParam->width, codecParam->height,
//                                        AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);

//    //把解码得到的损坏的像素数据剔除
//    sws_scale(imgConvertContext, frame->data, frame->linesize, 0, codecParam->height, outputDst, outputLineSize);
//    output.save("C:\\Users\\16409\\Desktop\\oneFrame.jpg");
//    sws_freeContext(imgConvertContext);
//    //av_frame_free(&frame);
    return output;
    qDebug() << "frameToImage";

//    QImage image(pFrame->width, pFrame->height, QImage::Format_RGB32);

//    // first convert the input AVFrame to the desired format
//    SwsContext* img_convert_ctx = sws_getContext(pFrame->width, pFrame->height,
//                                                 (AVPixelFormat)pFrame->format,
//                                                 pFrame->width, pFrame->height, AV_PIX_FMT_RGB32,
//                                                 SWS_BICUBIC, NULL, NULL, NULL);
//    if(!img_convert_ctx){
//        qDebug() << "Failed to create sws context";
//        return QImage();
//    }

//    // prepare line sizes structure as sws_scale expects
//    int rgb_linesizes[4];
//    av_image_fill_linesizes(rgb_linesizes, AV_PIX_FMT_RGB32, pFrame->width);
//    // prepare char buffer in array, as sws_scale expects
//    uint8_t *rgbData[] = {image.bits()};

//    if(sws_scale(img_convert_ctx, pFrame->data, pFrame->linesize, 0,
//                pFrame->height, rgbData, rgb_linesizes) != pFrame->height){
//        qDebug() << "Error changing frame color range";
//        free(rgbData[0]);
//        sws_freeContext(img_convert_ctx);
//        return QImage();
//    }
//    sws_freeContext(img_convert_ctx);
//    image.save(QString("C:\\Users\\16409\\Desktop\\rgbPicture.png"),"PNG");
//    return image;

    return * new QImage();

}
