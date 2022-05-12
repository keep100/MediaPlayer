#include "XAudioPlay.h"
#include <QAudioFormat>
#include <QAudioSink>
#include <iostream>
#include <mutex>
class CXAudioPlay :public XAudioPlay
{
public:
    QAudioSink *output = NULL;
    QIODevice *io = NULL;
    std::mutex mux;
    virtual void Clear()
    {
        mux.lock();
        if (io)
        {
            io->reset();
        }
        mux.unlock();
    }
    virtual void Close()
    {
        mux.lock();

        if (io)
        {
            io->close ();
            io = NULL;
        }
        if (output)
        {
            output->stop();
            delete output;
            output = 0;
        }
        mux.unlock();
    }
    virtual long long GetNoPlayMs()
    {
        mux.lock();
        if (!output)
        {
            mux.unlock();
            return 0;
        }
        long long pts = 0;
        //还未播放的字节数
        double size = output->bufferSize() - output->bytesFree();
        //一秒音频字节大小
        double secSize = sampleRate * (16 / 8) *channels;
        if (secSize <= 0)
        {
            pts = 0;
        }
        else
        {
            pts = (size/secSize) * 1000;
        }
        mux.unlock();
        return pts;
    }

    virtual bool Open()
    {
        Close();
        QAudioFormat fmt;
        fmt.setSampleRate(sampleRate);
        fmt.setChannelCount(channels);
        fmt.setSampleFormat((QAudioFormat::SampleFormat)sampleFormat);
        mux.lock();
        output = new QAudioSink(fmt);
        //output->setBufferSize(1024 * 10);
        io = output->start(); //开始播放
        mux.unlock();
        if(io)
            return true;
        return false;
    }
    void SetPause(bool isPause)
    {
        mux.lock();
        std::cout << "ap->isPause()" << std::endl;
        if (!output)
        {
            mux.unlock();
            return;
        }
        if (isPause)
        {
            output->suspend();
        }
        else
        {
            output->resume();
        }
        mux.unlock();
    }
    virtual bool Write(const unsigned char *data, int datasize)
    {
        if (!data || datasize <= 0)return false;
        mux.lock();
        if (!output || !io)
        {
            mux.unlock();
            return false;
        }
        int size = io->write((char *)data, datasize);
        mux.unlock();
        if (datasize != size)
            return false;
        return true;
    }
    virtual int GetFree()
    {
        mux.lock();
        if (!output)
        {
            mux.unlock();
            return 0;
        }
        int free = output->bytesFree();
        mux.unlock();
        return free;
    }
};
XAudioPlay *XAudioPlay::Get()
{
    static CXAudioPlay play;
    return &play;
}


XAudioPlay::XAudioPlay()
{

}
XAudioPlay::~XAudioPlay()
{

}
