#include "XDecode.h"
#include "XSubtileThread.h"
#include "XVideoThread.h"
#include <QFile>
#include <QFileInfo>
bool XSubtileThread::Open(AVCodecParameters *para) {
    if (!para)
        return false;
    Clear();
    vmux.lock();
    codecParam = para;
    vmux.unlock();
    int re = true;
    if (!decode->Open(para))
        re = false;
    if (vt != nullptr) {
        AVCodecContext *videoCodecContext = vt->decode->codec;
        QString args = QString::asprintf(
                    "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d", videoCodecContext->width,
                    videoCodecContext->height, videoCodecContext->pix_fmt, vt->timebase.num, vt->timebase.den,
                    videoCodecContext->sample_aspect_ratio.num,
                    videoCodecContext->sample_aspect_ratio.den);
        qDebug() << "Video Args: " << args;

        vt->subtitleOpened = false;
        if (re) {
            //字幕流直接用视频名即可
            QString subtitleFilename = vt->fileName;
            subtitleFilename.replace('/', "\\\\");
            subtitleFilename.insert(subtitleFilename.indexOf(":\\"), char('\\'));
            QString filterDesc = QString("subtitles=filename='%1':original_size=%2x%3")
                    .arg(subtitleFilename)
                    .arg(videoCodecContext->width)
                    .arg(videoCodecContext->height);
            qDebug() << "Filter Description:" << filterDesc.toStdString().c_str();
            vt->subtitleOpened = init_subtitle_filter(vt->buffersrcContext, vt->buffersinkContext,
                                                      args, filterDesc);
            if (!vt->subtitleOpened)
                qDebug() << "字幕打开失败!";
        }
        else {
            //没有字幕流时，在同目录下寻找字幕文件
            //字幕相关，使用srt, ssa, ass, lrc都行
            int suffixLength = QFileInfo(vt->fileName).suffix().length();
            QString subtitleFilename =
                    vt->fileName.mid(0, vt->fileName.length() - suffixLength - 1);
            QString suffix[] = {".ass", ".ssa", ".lrc", ".srt"};
            bool subIsExist = false;
            // 判断是否存在字幕文件
            for (QString s : suffix) {
                if (QFile::exists(subtitleFilename + s)) {
                    subtitleFilename += s;
                    subIsExist = true;
                    break;
                }
            }
            if (subIsExist) {
                //初始化subtitle filter
                //绝对路径必须转成D\:\\xxx\\test.ass这种形式, 记住，是[D\:\\]这种形式
                // toNativeSeparator()无用，因为只是 / -> \ 的转换
                subtitleFilename.replace('/', "\\\\");
                subtitleFilename.insert(subtitleFilename.indexOf(":\\"), char('\\'));
                QString filterDesc =
                        QString("subtitles=filename='%1':original_size=%2x%3")
                        .arg(subtitleFilename)
                        .arg(videoCodecContext->width)
                        .arg(videoCodecContext->height);
                qDebug() << "Filter Description:" << filterDesc.toStdString().c_str();
                vt->subtitleOpened = init_subtitle_filter(vt->buffersrcContext, vt->buffersinkContext,
                                                          args, filterDesc);
                if (!vt->subtitleOpened)
                    qDebug() << "字幕打开失败!";
            }

        }
    }
    return re;
}

// 初始化字幕过滤器
bool XSubtileThread::init_subtitle_filter(AVFilterContext *&buffersrcContext,
                                          AVFilterContext *&buffersinkContext,
                                          QString args, QString filterDesc) {
    const AVFilter *buffersrc = avfilter_get_by_name("buffer");
    const AVFilter *buffersink = avfilter_get_by_name("buffersink");
    AVFilterInOut *output = avfilter_inout_alloc();
    AVFilterInOut *input = avfilter_inout_alloc();
    AVFilterGraph *filterGraph = avfilter_graph_alloc();
    // 释放过滤器输入输出的函数
    auto release = [&output, &input] {
        avfilter_inout_free(&output);
        avfilter_inout_free(&input);
    };

    if (!output || !input || !filterGraph) {
        release();
        return false;
    }

    //创建输入过滤器，需要arg
    if (avfilter_graph_create_filter(&buffersrcContext, buffersrc, "in",
                                     args.toStdString().c_str(), nullptr,
                                     filterGraph) < 0) {
        qDebug() << "Has Error: line =" << __LINE__;
        release();
        return false;
    }

    if (avfilter_graph_create_filter(&buffersinkContext, buffersink, "out",
                                     nullptr, nullptr, filterGraph) < 0) {
        qDebug() << "Has Error: line =" << __LINE__;
        release();
        return false;
    }

    output->name = av_strdup("in");
    output->next = nullptr;
    output->pad_idx = 0;
    output->filter_ctx = buffersrcContext;

    input->name = av_strdup("out");
    input->next = nullptr;
    input->pad_idx = 0;
    input->filter_ctx = buffersinkContext;

    if (avfilter_graph_parse_ptr(filterGraph, filterDesc.toStdString().c_str(),
                                 &input, &output, nullptr) < 0) {
        qDebug() << "Has Error: line =" << __LINE__;
        release();
        return false;
    }

    if (avfilter_graph_config(filterGraph, nullptr) < 0) {
        qDebug() << "Has Error: line =" << __LINE__;
        release();
        return false;
    }

    release();
    return true;
}


void XSubtileThread::SetPause(bool isPause) {
    vmux.lock();
    this->isPause = isPause;
    vmux.unlock();
}

void XSubtileThread::run() {
    while (!isExit) {
        vmux.lock();
        if (this->isPause) {
            vmux.unlock();
            msleep(5);
            continue;
        }
        AVPacket *pkt = Pop();
        AVSubtitle *sub = decode->RecvSubtile(pkt);

        vmux.unlock();
        msleep(1);
    }

}
