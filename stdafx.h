#ifndef STDAFX_H
#define STDAFX_H
#include <QTimer>
#include <QApplication>
#include <QDebug>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <QAudioFormat>
#include <QAudioSink>
#include <QIODevice>
#include <iostream>
extern "C" {
#include "libavformat/avformat.h"
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/frame.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h>
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/time.h>
}
#endif // STDAFX_H
