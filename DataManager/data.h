#ifndef DATA_H
#define DATA_H

#include <QObject>
#include<QDebug>
#include<QJsonObject>
#include"XMediaManager.h"
#include<QDir>

enum State{
    Normal,
    Miss,
    Error
};

class Data
{
    Q_GADGET
    Q_PROPERTY(QString fileName READ fileName)      //文件名(如果是音频且音频有标题就是标题否则则为文件名)
    Q_PROPERTY(QString album READ album)            //专辑名（如果没有则为""）
    Q_PROPERTY(QString artist READ artist)          //作者名
    Q_PROPERTY(QString filePath READ filePath)      //文件路径
    Q_PROPERTY(qint64 duration READ duration)       //持续时间
    Q_PROPERTY(qint64 lastTime READ duration)       //上次播放的时间
    Q_PROPERTY(int index READ index )               //索引
    Q_PROPERTY(bool isAudio READ isAudio )          //是否是音频
    Q_PROPERTY(QString imgPath READ imgPath)        //图片地址（可以通过图片地址是否为空来判断有无图片）

public:
    inline QString fileName(){return _fileName;}
    inline QString filePath(){return _filePath;}
    inline qint64 duration(){return _duration;}
    inline qint64 lastTime(){return _lastTime;}
    inline int index(){return _index;}
    inline bool isAudio(){return _isAudio;}
    inline QString imgPath(){return _imgPath;}
    inline QString album(){return _album;}
    inline QString artist(){return _artist;}

    State verify();                      //验证文件是否出错：1）判断文件是否存在 2）计算md5码
    void setLastTime(qint64 time);      //设置文件上次的播放时间
    void setIndex(int i){_index = i;}   //设置序号
    Data(){}
    Data(const BriefInfo& info,const QFileInfo&);
    Data(const QJsonObject& json);
    QJsonObject toJson();

public:
    static QDir dir;

private:
    QString _md5 = "";
    QString _fileName = "";
    QString _filePath = "";
    qint64 _duration = 0;
    qint64 _lastTime = 0;
    QString _imgPath = "";
    QString _album = "";
    QString _artist = "";
    bool _isAudio = false;
    int _index = 0;

};

#endif // DATA_H
