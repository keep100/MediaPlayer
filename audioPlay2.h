#ifndef AUDIOPLAY2_H
#define AUDIOPLAY2_H

#include <math.h>
#include <QFile>
#include <QAudioSink>
#include <QByteArray>
#include <QComboBox>
#include <QIODevice>
#include <QLabel>
#include <QMainWindow>
#include <QObject>
#include <QPushButton>
#include <QSlider>
#include <QTimer>
#include <QScopedPointer>
#include <QMediaDevices>
#include <mutex>

class audioPlay2
{
public:
    QScopedPointer<QAudioSink> m_audioOutput;

    audioPlay2();
    ~audioPlay2();

    bool Open(const QAudioDevice &deviceInfo);
    void Close();
    void Clear();
    void SetPause(bool isPause);
    long long GetNoPlayMs();
    bool Write(char* data, int datasize);
    int GetFree();
    void SetVolume(double volume);
    QAudioFormat GetFormat();
private:
    void volumeChanged(double);
    QIODevice *io = nullptr;
    QAudioFormat format;
    std::mutex mux;

};


#endif // AUDIOPLAY2_H

