
#include "audioPlay2.h"
#include <QAudioDevice>
#include <QAudioSink>
#include <QDebug>
#include <QVBoxLayout>
#include <qmath.h>
#include <qendian.h>
#include <QFile>



audioPlay2::audioPlay2(): m_devices(new QMediaDevices(this))
{
    qDebug()<<"audioPlay::audioPlay()";

}

audioPlay2::~audioPlay2()
{

}

bool audioPlay2::Open(){
    const QAudioDevice &deviceInfo = m_devices->defaultAudioOutput();
    format = deviceInfo.preferredFormat();
    m_audioOutput.reset(new QAudioSink(deviceInfo, format));
    qreal initialVolume = QAudio::convertVolume(m_audioOutput->volume(),
                                                QAudio::LinearVolumeScale,
                                                QAudio::LogarithmicVolumeScale);
    qDebug()<<"format: "<<format;
    qDebug()<<"initialVolume: "<<initialVolume;
//    m_audioOutput->stop();
//    m_audioOutput->resume();
    io = m_audioOutput->start();
    if(io)return true;
    else return false;
}

void audioPlay2::Close(){
    if(io){
        io->close();
        io = NULL;
    }
    if(m_audioOutput){
        m_audioOutput->stop();
        m_audioOutput->reset();
    }
}

void audioPlay2::Clear(){
    if(io){
        io->reset();
    }
}

void audioPlay2::SetPause(bool isPause){
    if(!m_audioOutput){
        return;
    }
    if(isPause){
        m_audioOutput->suspend();
    }else{
        m_audioOutput->resume();
    }
}

void audioPlay2::SetVolume(double volume){
    if(volume>=0&&volume<=1) volumeChanged(volume);
}

long long audioPlay2::GetNoPlayMs(){
    if(!m_audioOutput)return 0;
    long long pts = 0;
    double size = m_audioOutput->bufferSize() - m_audioOutput->bytesFree();
    double secSize = format.sampleRate()*format.bytesPerSample()*format.channelCount();
    if(secSize<=0) pts = 0;
    else pts = (size/secSize)*1000;
    return pts;
}

bool audioPlay2::Write(char* data, int datasize){
    if(!data||datasize<=0)return false;
    mux.lock();
    if(!m_audioOutput||!io){
        mux.unlock();
        return false;
    }
    int size = io->write(data, datasize);
    mux.unlock();
    if(datasize != size)return false;
    else return true;
}

int audioPlay2::GetFree(){
    mux.lock();
    if(!m_audioOutput){
        mux.unlock();
        return 0;
    }
    int free = m_audioOutput->bytesFree();
    mux.unlock();
    return free;
}

QAudioFormat audioPlay2::GetFormat(){
    return format;
}

void audioPlay2::volumeChanged(double v)
{
    qreal linearVolume = QAudio::convertVolume(v, QAudio::LogarithmicVolumeScale,
                                               QAudio::LinearVolumeScale);
    m_audioOutput->setVolume(linearVolume);
}


