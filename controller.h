#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QDebug>
#include "playMode.h"


//单例，提供方法给qml调用，并根据情况发出对应的信号
class Controller : public QObject
{
    Q_OBJECT
    Q_PROPERTY(PlayMode::mode mode READ mode WRITE setMode NOTIFY modeChanged)      //播放状态
    Q_PROPERTY(bool isAudio READ isAudio WRITE setIsAudio NOTIFY isAudioChanged)    //是否是音频
private:
    PlayMode::mode _mode = PlayMode::Loop;

public:
    static Controller* getInstance();       //获取实例
    Q_INVOKABLE void play(int index);       //开始播放
    Q_INVOKABLE void pause();               //暂停播放
    Q_INVOKABLE void importData(QList<QString> list);       //导入数据
    Q_INVOKABLE void deleteData(qint64 index);              //删除数据
    Q_INVOKABLE void setVoice(int voice);   //调整音量
    Q_INVOKABLE void next();    //下一首
    Q_INVOKABLE void pre();     //上一首
    Q_INVOKABLE void setState();//设置播放模式
    Q_INVOKABLE void exitPlay();//退出播放

public:
    PlayMode::mode mode();
    bool isAudio();
    void setIsAudio(bool state);
    void setMode(PlayMode::mode m);


private:
    static Controller* _controller;
    explicit Controller(QObject *parent = nullptr);

signals:
    void modeChanged();
    void isAudioChanged();
};

#endif // CONTROLLER_H
