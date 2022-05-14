#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QDebug>
#include "PlayMode.h"


//提供方法给qml调用，并根据情况发出对应的信号
class Controller : public QObject
{
    Q_OBJECT
    Q_PROPERTY(PlayMode::mode mode READ mode WRITE setMode NOTIFY modeChanged)      //播放状态
    Q_PROPERTY(float playSpeed READ playSpeed WRITE setPlaySpeed NOTIFY playSpeedChanged)//播放速度
    Q_PROPERTY(float voice READ voice WRITE setVoice NOTIFY voiceChanged) //音量

public:
    explicit Controller(QObject *parent = nullptr);
    Q_INVOKABLE void stopVideo();           //暂停播放
    Q_INVOKABLE void stopAudio();           //暂停播放
    Q_INVOKABLE void startPlayVideo(int index);     //开始播放视频
    Q_INVOKABLE void startPlayAudio(int index);     //开始播放音频
    Q_INVOKABLE void importAudioData(QList<QString> list);   //导入音频数据
    Q_INVOKABLE void deleteAudioData(qint64 index);          //删除音频数据
    Q_INVOKABLE void importVideoData(QList<QString> list);   //导入视频数据
    Q_INVOKABLE void deleteVideoData(qint64 index);          //删除视频数据
    Q_INVOKABLE void playNextVideo();   //下一个
    Q_INVOKABLE void playPreVideo();    //上一个
    Q_INVOKABLE void playNextAudio();   //下一首
    Q_INVOKABLE void playPreAudio();    //上一首
    Q_INVOKABLE void exit();            //退出播放
    Q_INVOKABLE void searchAudio(QString);  //搜索音频
    Q_INVOKABLE void searchVideo(QString);  //搜索视频

signals:
    void modeChanged(PlayMode::mode);       //播放模式改变
    void playSpeedChanged(float);           //播放速度改变
    void playAudio(int);                    //播放指定索引的音频
    void playVideo(int);                    //播放指定索引的视频
    void importAudio(const QList<QString> &); //导入音频
    void deleteAudio(int);                  //删除音频
    void importVideo(const QList<QString> &); //导入视频
    void deleteVideo(int);                  //删除视频
    void nextAudio();                       //下一个音频
    void nextVideo();                       //下一个视频频
    void preVideo();                        //上一首
    void preAudio();                        //下一首
    void voiceChanged(int);                 //音量改变
    void exitPlay();                        //退出播放
    void pause();                           //暂停
    void queryAudio(const QString&);        //搜索音频
    void queryVideo(const QString&);        //搜索视频

public:
    //get
    PlayMode::mode mode();
    float voice();
    float playSpeed();

    //set
    void setMode(PlayMode::mode m);
    void setPlaySpeed(float s);
    void setVoice(float v);

private:
    PlayMode::mode _mode = PlayMode::Loop;
    float _playSpeed = 1.0f;
    int _voice = 15;

};

#endif // CONTROLLER_H
