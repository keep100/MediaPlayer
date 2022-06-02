#include "XDecode.h"
#include "XSubtitleThread.h"
#include "XVideoThread.h"
#include <QFileInfo>
#include <QDir>
#include <QStringList>

bool XSubtitleThread::Open(AVCodecParameters *para) {
    // 重置过滤器以及将字幕判断标志置0
    if (vt->buffersrcContext != nullptr)
        avfilter_free(vt->buffersrcContext);
    if (vt->buffersinkContext != nullptr)
        avfilter_free(vt->buffersinkContext);
    vt->buffersrcContext = vt->buffersinkContext = nullptr;
    vt->subtitleOpened = false;

    assert (vt != nullptr);

    AVCodecContext *videoCodecContext = vt->decode->codec;
    QString args = QString::asprintf(
                "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d", videoCodecContext->width,
                videoCodecContext->height, videoCodecContext->pix_fmt, vt->timebase.num, vt->timebase.den,
                videoCodecContext->sample_aspect_ratio.num,
                videoCodecContext->sample_aspect_ratio.den);
    qDebug() << "video_args" << args;
    Clear();

    // 判断是否存在字幕流
    if (para) {
        vmux.lock();
        codecParam = para;
        vmux.unlock();
        if (decode->Open(para)) {
            // 字幕流直接用视频名即可
            QString subtitleFilename = vt->fileName;
            subtitleFilename.replace('/', "\\\\");
            subtitleFilename.insert(subtitleFilename.indexOf(":\\"), char('\\'));
            QString filterDesc = QString("subtitles=filename='%1':original_size=%2x%3")
                    .arg(subtitleFilename)
                    .arg(videoCodecContext->width)
                    .arg(videoCodecContext->height);
            vt->subtitleOpened = init_subtitle_filter(vt->buffersrcContext, vt->buffersinkContext,
                                                      args, filterDesc);
        }
    }
    //没有字幕流时，在同目录下寻找字幕文件
    else {
        QString subtitleFilename;
        QFileInfo fileInfo(vt->fileName);
        QString fileName = fileInfo.completeBaseName();

        // 判断是否存在字幕文件
        QDir dir(fileInfo.absoluteDir());
        QStringList nameFilter;
        // 字幕相关，使用srt, ssa, ass, lrc都行
        // 只要字幕文件名与视频名部分匹配即可
        nameFilter << (fileName + "*.ass") <<  (fileName + "*.ssa")
                   << (fileName +  "*.lrc") << (fileName +  "*.srt");
        dir.setNameFilters(nameFilter);
        QStringList infolist = dir.entryList(nameFilter, QDir::Files | QDir::Dirs, QDir::Name);

        bool subtitleExist = false;
        for (int i = 0; i < infolist.size(); i++) {
            QString s = infolist.at(i);
            // 只要存在符合一个条件的字幕即break
            if (dir.exists(s)) {
                subtitleExist = true;
                subtitleFilename = dir.absoluteFilePath(s);
                break;
            }
        }

        if (subtitleExist) {
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
            vt->subtitleOpened = init_subtitle_filter(vt->buffersrcContext, vt->buffersinkContext,
                                                      args, filterDesc);
        }
    }

    return vt->subtitleOpened;
}


// 初始化字幕过滤器
bool XSubtitleThread::init_subtitle_filter(AVFilterContext *&buffersrcContext,
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
