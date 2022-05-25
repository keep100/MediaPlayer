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

class UserInfo{
    Q_GADGET
    Q_PROPERTY(QString bckPath READ bckPath)            //背景图片路径
    Q_PROPERTY(float blurRatio READ blurRatio)          //模糊度
public:

    inline QString bckPath(){return _bckPath;}
    inline float blurRatio(){return _blurRatio;}

    void setBckPath(QString path){
        _bckPath = path;
    }

    void setBlurRatio(float r){
        _blurRatio = r;
    }

    UserInfo(){

    }
    void init(const QJsonObject& json){
        _bckPath = json["bckPath"].toString();
        _blurRatio = json["blurRatio"].toDouble();
    }
    QJsonObject toJson(){
        QJsonObject json;
        json.insert("bckPath",_bckPath);
        json.insert("blurRatio",_blurRatio);
        return json;
    }

private:
    QString _bckPath = "";
    float _blurRatio = 0.5;

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
    Q_PROPERTY(QString codecId READ codecId)        //编码格式
    Q_PROPERTY(int aBitRate READ aBitRate)          //音频比特率
    Q_PROPERTY(int vBitRate READ vBitRate)          //视频比特率
    Q_PROPERTY(int channels READ channels)          //声道数
    Q_PROPERTY(int width READ width)                //视频的宽
    Q_PROPERTY(int height READ height)              //视频的高

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
    inline QString codecId(){return _codecId;}
    inline int width(){return _width;}
    inline int height(){return _height;}
    inline int channels(){return _channels;}
    inline int aBitRate(){return _aBitRate;}
    inline int vBitRate(){return _vBitRate;}

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
    QString _codecId;
    int _width = 0;
    int _height = 0;
    int _aBitRate = 0;
    int _vBitRate = 0;
    int _channels = 0;

};

#endif // DATA_H
