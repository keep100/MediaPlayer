#ifndef DATA_H
#define DATA_H

#include <QObject>
#include<QDebug>
#include<QJsonObject>

class Data
{
    Q_GADGET

    Q_PROPERTY(QString fileName READ fileName)
    Q_PROPERTY(QString filePath READ filePath)
    Q_PROPERTY(qint64 duration READ duration)
    Q_PROPERTY(qint64 lastTime READ duration)

public:
    inline QString fileName(){return _fileName;}
    inline QString filePath(){return _filePath;}
    inline qint64 duration(){return _duration;}
    inline qint64 lastTime(){return _lastTime;}

    bool verify();  //验证文件是否出错：1）判断文件是否存在 2）计算md5码
    void setLastTime(qint64 time);  //设置文件上次的播放时间
    Data(){}
    Data(const QString& path);
    Data(const QJsonObject& json);
    QJsonObject toJson();


private:
    QString _md5 = "";
    QString _fileName = "";
    QString _filePath = "";
    qint64 _duration = 0;
    qint64 _lastTime = 0;

};

#endif // DATA_H
