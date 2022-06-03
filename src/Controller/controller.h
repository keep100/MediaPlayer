#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include "PlayMode.h"
#include "src/DataManager/datamanager.h"
#include "utils/yuvdata.h"


//提供方法给qml调用，并根据情况发出对应的信号
class Controller : public QObject
{
    Q_OBJECT
    Q_PROPERTY(PlayMode::mode mode READ mode WRITE setMode NOTIFY modeChanged)      //播放状态
    Q_PROPERTY(float playSpeed READ playSpeed WRITE setPlaySpeed NOTIFY playSpeedChanged)//播放速度
    Q_PROPERTY(float voice READ voice WRITE setVoice NOTIFY voiceChanged)   //音量
    Q_PROPERTY(int time READ time NOTIFY timeChanged)         //时间

public:


    //供前端调用（前端--》控制器）
    Q_INVOKABLE void stop();                                                //暂停播放
    Q_INVOKABLE void startPlay(int index, bool isAudio);                    //开始播放视频
    Q_INVOKABLE void importData(const QList<QString> &list, bool isAudio);  //导入音频数据
    Q_INVOKABLE void deleteData(int index, bool isAudio);                   //删除数据
    Q_INVOKABLE void playNext(bool isAudio);                                //下一个
    Q_INVOKABLE void playPre(bool isAudio);                                 //上一个
    Q_INVOKABLE void exit();                                                //退出播放
    Q_INVOKABLE void setTime(int t);             //设置播放时间（当前端拖动完进度条之后就调用）
    Q_INVOKABLE void setUserInfo(QString,float);                            //设置用户数据

signals:
    //不需要主动连接（控制器--》前端）
    void modeChanged(PlayMode::mode);       //播放模式改变
    void timeChanged();                     //时间改变（后端--》前端,用于通知前端更新进度条）

    //需要前端主动连接（控制器--》前端）
    void fileMiss(Data path);                   //文件缺失
    void fileError(Data path);                  //文件错误(md5对不上，或者解析失败）
    void fileImportFail(const QString &path);   //文件导入失败
    void fileFinish();                          //当前文件播放结束
    void update(std::shared_ptr<YUVData>);      //通知渲染组件进行更新

    //需要后端连接（控制器--》后端）
    void playSpeedChanged(float);           //播放速度改变
    void voiceChanged(int);                 //音量改变
    void exitPlay();                        //退出播放
    void pause();                           //暂停
    void skipTime(double time);             //时间跳转（前端拖动进度条导致）

    //前后端都连接（通知前端开始准备播放，后端开始解码）
    void playMedia(const QString &path);    //播放

public:
    static Controller* controller;
    static DataManager manager;
    static void init(QQmlApplicationEngine& engine);

    //get
    inline PlayMode::mode mode()
    {
        return _mode;
    }
    inline float voice()
    {
        return _voice;
    }
    inline float playSpeed()
    {
        return _playSpeed;
    }
    inline int time()
    {
        return _time;
    }

    //set
    void setMode(PlayMode::mode m);
    void setPlaySpeed(float s);
    void setVoice(float v);

    //需要与其他部分进行连接
    void updateYUV(std::shared_ptr<YUVData> data);       //连接后端传来的信号
    void updateTime(int time);
    void finish();          //连接后端发来的结束函数

private:
    explicit Controller(QObject *parent = nullptr){}

private:
    PlayMode::mode _mode = PlayMode::Loop;
    float _playSpeed = 1.0f;
    float _voice = 50;
    int _time = 0;
};

#endif // CONTROLLER_H
