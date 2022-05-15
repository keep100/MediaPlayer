#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include "data.h"
#include <QMap>
#include "playMode.h"

enum State{
    Normal,
    Miss,
    Error
};

class DataManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList videoList READ videoList NOTIFY videoListChanged)
    Q_PROPERTY(QVariantList audioList READ audioList NOTIFY audioListChanged)
    Q_PROPERTY(Data curVideo READ curVideo NOTIFY curVideoChanged)
    Q_PROPERTY(Data curAudio READ curAudio NOTIFY curAudioChanged)
public:
    explicit DataManager(QObject *parent = nullptr);
    QVariantList videoList();
    QVariantList audioList();
    Data curVideo();
    Data curAudio();

signals:
    void videoListChanged();
    void audioListChanged();
    void curVideoChanged();
    void curAudioChanged();

public:
    State importVideo(const QString&);          //导入视频
    State importAudio(const QString&);          //导入音频
    void deleteVideo(int);                      //删除指定索引的视频
    void deleteAudio(int);                      //删除指定索引的音频
    State checkVideo(int);                      //检查视频文件
    State checkAudio(int);                      //检查音频文件
    int nextVideo();                            //下一个视频
    int nextAudio();                            //下一个音频
    int preVideo();                             //上一个视频
    int preAudio();                             //上一个音频
    Data getVideo(int);                         //获取当前视频
    Data getAudio(int);                         //获取当前音频
    void recordVideo(int time);                 //记录当前播放历史
    void setCurVideo(int);                      //设置当前视频
    void setCurAudio(int);                      //设置当前音频
    void setMode(PlayMode::mode m);             //设置播放模式
    ~DataManager();                             //析构函数

private:
    QList<Data> _videoList,_audioList;
    PlayMode::mode mode;
    int videoIndex = -1,audioIndex = -1;

};

#endif // DATAMANAGER_H
