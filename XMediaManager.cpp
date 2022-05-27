#include "XMediaManager.h"
#include "XAudioThread.h"
#include "XDemux.h"
#include "XDemuxThread.h"
#include "XVideoThread.h"
extern "C" {
#include "libavcodec/avcodec.h"
#include "libavdevice/avdevice.h"
#include "libavformat/avformat.h"
#include "libavformat/avio.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
}
#include "controller.h"
#include <QDebug>
#include <QThread>
#include <iostream>

using namespace std;

void XMediaManager::bind(QObject *obj) {
    Controller *ctrl = dynamic_cast<Controller *>(obj);
    if (ctrl != nullptr) {
        QObject::connect(ctrl, &Controller::playMedia, this, &XMediaManager::playMedia);
        QObject::connect(ctrl, &Controller::pause, this, &XMediaManager::toggle);
        QObject::connect(ctrl, &Controller::exitPlay, this, &XMediaManager::end);
        QObject::connect(ctrl, &Controller::voiceChanged, this, &XMediaManager::setVolume);
        QObject::connect(ctrl, &Controller::skipTime, this,&XMediaManager::seek);
    }
}

BriefInfo XMediaManager::getBriefInfo(const QString& url) {
    BriefInfo briefInfo;
    if (url.isNull()||url.isEmpty())
        return briefInfo;
    qDebug()<<url;
    XDemux *demux = new XDemux();
    XDecode *decode = new XDecode();
    auto temp = url.toUtf8();
    qDebug()<<"temp"<<temp;
    demux->Open(temp.data());

    briefInfo.totalMs = demux->totalMs;
    briefInfo.channels = demux->channels;
    briefInfo.width = demux->width;
    briefInfo.height = demux->height;
    briefInfo.aBitRate = demux->CopyAPara()->bit_rate;

    QString suffix = QString(url).split(".").last();
    qDebug()<<suffix;
    if (QString("mp4, flv, avi, mkv").contains(suffix)) { //是视频就提取缩略图

        briefInfo.mediaType = "video";
        briefInfo.codecId = avcodec_get_name(demux->CopyVPara()->codec_id);
        briefInfo.vBitRate = demux->CopyVPara()->bit_rate;
        decode->Open(demux->CopyVPara());
        AVFrame *pFrame = nullptr;
        demux->Seek(0.5);
        //先recv，后send
        for(int i=0;i<10;)
        {
            pFrame = decode->Recv();
            qDebug()<<888;
            if (pFrame && pFrame->key_frame) { // receive
                QImage&& temp = getQImageFromFrame(pFrame, demux->CopyVPara());
                if (!temp.isNull() && temp.bits()) {
                    int w = temp.width(), h = temp.height();
                    int rgbValue = (((temp.pixel(0.25 * w, 0.25 * h) +
                                      temp.pixel(0.75 * w, 0.75 * h) +
                                      temp.pixel(0.25 * w, 0.75 * h) +
                                      temp.pixel(0.75 * w, 0.25 * h)) / 4));
                    int means = (qRed(rgbValue) + qGreen(rgbValue) + qBlue(rgbValue)) / 3;
                    if (means > 50) {
                        qDebug() << "有东西的图";
                        break;
                    }
                    if(i>=10)break;
                    i++;
                }
            }

            AVPacket *pkt = demux->ReadVideo(); // send
            decode->Send(pkt);
        }
        briefInfo.img = getQImageFromFrame(pFrame, demux->CopyVPara());

    }
    else if (QString("mp3, flac, wav, m4a").contains(suffix)) { //是音频就提取音频的相关信息
        briefInfo.mediaType = "audio";
        briefInfo.codecId = avcodec_get_name(demux->CopyAPara()->codec_id);
        qDebug()<<"briefInfo"<<briefInfo.mediaType<<briefInfo.codecId;
        AVFormatContext *m_AVFormatContext = NULL;
        int result = avformat_open_input(&m_AVFormatContext, url.toUtf8().data(), nullptr, nullptr);
        if (result != 0 || m_AVFormatContext == nullptr) {
            qDebug() << "fffff1"<<(result != 0);
            return briefInfo;
        }

        // 查找流信息，把它存入AVFormatContext中
        result = avformat_find_stream_info(m_AVFormatContext, nullptr);
        if (result) {
            qDebug() << "fffff2";
            return briefInfo;
        }

        int streamsCount = m_AVFormatContext->nb_streams;

        // 读取详细信息
        QMap<QString, QString> mInfoMap;
        QImage mInfoImage;
        AVDictionaryEntry *tag = nullptr;
        while (tag = av_dict_get(m_AVFormatContext->metadata, "", tag, AV_DICT_IGNORE_SUFFIX)) {
            QString keyString = tag->key;
            QString valueString = QString::fromUtf8(tag->value);
            mInfoMap.insert(keyString.toLower(), valueString);
        }
        briefInfo.album = mInfoMap["album"];
        briefInfo.artist = mInfoMap["artist"];
        briefInfo.title = mInfoMap["title"];
        qDebug()<<mInfoMap;
        for (int i = 0; i < streamsCount; ++i) {
            if (m_AVFormatContext->streams[i]->disposition & AV_DISPOSITION_ATTACHED_PIC) {
                qDebug() << "-----" << i;
                AVPacket pkt = m_AVFormatContext->streams[i]->attached_pic;
                mInfoImage = QImage::fromData((uchar *)pkt.data, pkt.size);
            }
        }
        briefInfo.img = mInfoImage;
    }

    return briefInfo;
}

QImage XMediaManager::getQImageFromFrame(const AVFrame *frame, AVCodecParameters *para) {

    QImage output(frame->width, frame->height+1,
                  QImage::Format_RGB32); //构造一个QImage用作输出
//    qDebug()<<45;
    int outputLineSize[4]; //构造AVFrame到QImage所需要的数据
    av_image_fill_linesizes(outputLineSize, AV_PIX_FMT_RGB32, para->width);
    uint8_t *outputDst[] = {output.bits()};
    //构造一个格式转换上下文
    SwsContext *imgConvertContext = sws_getContext(
                para->width, para->height, (AVPixelFormat)para->format, para->width,
                para->height, AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);
    qDebug()<<"briefInfo.vBitRate"<<455;
    //把解码得到的损坏的像素数据剔除
    sws_scale(imgConvertContext, frame->data, frame->linesize, 0, para->height, outputDst, outputLineSize);
    //    qDebug()<<frame->linesize[0];
    output.save("aaa.jpg");
    //    qDebug()<<para->height<<' '<<frame->height<<' '<<para->width<<' '<<frame->width;
    sws_freeContext(imgConvertContext);
    //    av_frame_free(&frame);

    output = output.copy(0,1,output.width(),output.height()-1);
//    qDebug()<<"output.size()"<<output.size();
    return output;
}

XMediaManager::XMediaManager() {
    _curState = INITIAL;
    if (!demuxThread)
        demuxThread = new XDemuxThread();
    demuxThread->Clear();
    bind(Controller::controller);
}

void XMediaManager::playMedia(QString url) {
    if (open(url)) {
        qDebug() << "XMediaManager::playMedia open succeed";
        play();
    }else{
        qDebug() << "XMediaManager::playMedia open failed";
        end();
    }
}

bool XMediaManager::open(QString url) {
    end();
    qDebug()<<"XMediaManager::open"<<url.toUtf8();
    if (_curState == INITIAL || _curState == END) {
        _curState = READY;
        if (!demuxThread)
            demuxThread = new XDemuxThread();
        demuxThread->SetPause(false);
        return demuxThread->Open(url.toUtf8().data(), NULL);

    } else {
        _curState = INITIAL;
        qDebug() << "error at XMediaManager::open()";
        return false;
    }
    return false;
}

void XMediaManager::play() {
    qDebug()<<"XMediaManager::play";
    if (_curState == READY) {
        _curState = PLAYING;
        if (!demuxThread) {
            _curState = END;
            qDebug() << "error at XMediaManager::play()1";
            return;
        }
        demuxThread->Start();
    }  else {
        _curState = INITIAL;
        qDebug() << "error at XMediaManager::play()2";
        return;
    }
}

void XMediaManager::toggle() {
    qDebug()<<"XMediaManager::toggle()"<<(_curState == PLAYING)<<(_curState == PAUSED);
    if (_curState == PLAYING) {
        _curState = PAUSED;
        if (!demuxThread) {
            _curState = END;
            qDebug() << "error at XMediaManager::toggle()1";
            return;
        }
        demuxThread->SetPause(true);
    } else if (_curState == PAUSED) {
        _curState = PLAYING;
        if (!demuxThread) {
            _curState = END;
            qDebug() << "error at XMediaManager::toggle()2";
            return;
        }
        demuxThread->SetPause(false);
    }else {
        _curState = INITIAL;
        qDebug() << "error at XMediaManager::toggle()3";
        return;
    }
}

void XMediaManager::setVolume(float v) {
    if (!demuxThread) {
        _curState = END;
        qDebug() << "error at XMediaManager::setVolume()";
        return;
    }
    qDebug()<<"XMediaManager::setVolume"<<v/100.0;
    demuxThread->SetVolume(v / 100.0);
}

void XMediaManager::seek(double pos) {
    qDebug()<<"XMediaManager::seek"<<pos;
    if (_curState == PLAYING || _curState == PAUSED) {
        if (!demuxThread) {
            _curState = END;
            qDebug() << "error at XMediaManager::seek()1";
            return;
        }
        demuxThread->Seek(pos);

    } else {
        _curState = INITIAL;
        qDebug() << "error at XMediaManager::seek()2";
        return;
    }
}

void XMediaManager::end() {
    _curState = END;
    if (demuxThread) {
        demuxThread->SetPause(true);
        demuxThread->Clear();
//        demuxThread->Close();
//        delete demuxThread;
//        demuxThread = nullptr;
    }
    qDebug()<<"XMediaManager::end()";
    return;
}
