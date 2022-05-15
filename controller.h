#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QDebug>
#include "playMode.h"
#include "DataManager/datamanager.h"


//提供方法给qml调用，并根据情况发出对应的信号
class Controller : public QObject
{
    Q_OBJECT
    Q_PROPERTY(PlayMode::mode mode READ mode WRITE setMode NOTIFY modeChanged)      //播放状态
    Q_PROPERTY(float playSpeed READ playSpeed WRITE setPlaySpeed NOTIFY playSpeedChanged)//播放速度
    Q_PROPERTY(float voice READ voice WRITE setVoice NOTIFY voiceChanged)   //音量
    Q_PROPERTY(int time READ time NOTIFY timeChanged)         //时间

public:
    explicit Controller(QObject *parent = nullptr);

    //供前端调用（前端--》控制器）
    Q_INVOKABLE void stopVideo();       //暂停播放
    Q_INVOKABLE void stopAudio();       //暂停播放
    Q_INVOKABLE void startPlayVideo(int index);     //开始播放视频
    Q_INVOKABLE void startPlayAudio(int index);     //开始播放音频
    Q_INVOKABLE void importAudio(const QList<QString>& list);   //导入音频数据
    Q_INVOKABLE void importVideo(const QList<QString>& list);   //导入视频数据
    Q_INVOKABLE void deleteAudio(int index);        //删除音频数据
    Q_INVOKABLE void deleteVideo(int index);        //删除视频数据
    Q_INVOKABLE void playNextVideo();   //下一个
    Q_INVOKABLE void playPreVideo();    //上一个
    Q_INVOKABLE void playNextAudio();   //下一首
    Q_INVOKABLE void playPreAudio();    //上一首
    Q_INVOKABLE void exit();            //退出播放
    Q_INVOKABLE void setTime(int t);    //设置播放时间（当前端拖动完进度条之后就调用）

signals:
    //不需要主动连接（控制器--》前端）
    void modeChanged(PlayMode::mode);       //播放模式改变
    void timeChanged();                     //时间改变（后端--》前端,用于通知前端更新进度条）

    //需要前端主动连接（控制器--》前端）
    void fileMiss(const QString& path);     //文件缺失
    void fileError(const QString& paht);    //文件错误

    //需要后端连接（控制器--》后端）
    void playSpeedChanged(float);           //播放速度改变
    void voiceChanged(int);                 //音量改变
    void exitPlay();                        //退出播放
    void pause();                           //暂停
    void skipTime(int time);                //时间跳转（前端拖动进度条导致）
    void playMedia(const QString& path);    //播放

public:

    //get
    inline PlayMode::mode mode(){return _mode;}
    inline float voice(){return _voice;}
    inline float playSpeed(){return _playSpeed;}
    inline int time(){return _time;}

    //set
    void setMode(PlayMode::mode m);
    void setPlaySpeed(float s);
    void setVoice(float v);

private:
    DataManager manager;
    PlayMode::mode _mode = PlayMode::Loop;
    float _playSpeed = 1.0f;
    int _voice = 15;
    int _time = 0;
};

#endif // CONTROLLER_H
