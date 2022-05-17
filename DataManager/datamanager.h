#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include "data.h"
#include <QMap>
#include "playMode.h"


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
    State importData(const QString&,bool isAudio);  //导入视频
    void deleteData(int,bool isAudio);              //删除指定索引的视频
    State check(int,bool isAudio);                  //检查文件
    int next();                                     //下一个（返回索引）
    int pre();                                      //上一个
    Data getData(int,bool isAudio);                 //通过索引获取指定文件的数据
    void recordVideo(long long time);               //记录当前播放历史（播放切换时调用，用于保留历史）
    void setCur(int,bool isAudio);                  //开始播放时调用，用于记录当前播放哪个文件
    void setMode(PlayMode::mode m);                 //设置播放模式
    void readData();                                //加载初始数据
    void writeData();                               //将数据写回文件中
    ~DataManager();                                 //析构函数

private:
    bool _isAudio = false;
    QList<Data> _videoList,_audioList;
    QList<int> _videoOrder,_audioOrder;             //播放顺序（随机播放）
    PlayMode::mode mode;
    int curIndex;                                   //当前对应着数据列表中的哪一项
    int curOrder = -1;                              //当前对应着顺序列表中哪一项（随机播放）
    QDir dir;
};

#endif // DATAMANAGER_H
