#include "XAudioPlay.h"
#include <QAudioFormat>
#include <QAudioSink>
#include <mutex>
class CXAudioPlay :public XAudioPlay
{
public:
    QAudioSink *output = NULL;
    QIODevice *io = NULL;
    std::mutex mux;
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
    virtual bool Open()
    {
        Close();
        QAudioFormat fmt;
        fmt.setSampleRate(sampleRate);
        fmt.setChannelCount(channels);
        fmt.setSampleFormat((QAudioFormat::SampleFormat)sampleFormat);
        mux.lock();
        output = new QAudioSink(fmt);
        io = output->start(); //开始播放
        mux.unlock();
        if(io)
            return true;
        return false;
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
