#include "data.h"
#include<QFile>
#include<QCryptographicHash>
#include<QFileInfo>

QDir Data::dir = QDir("./thumpnail");

//获取文件md5码，只获取前2048个字节
QString getFileMd5(const QString &path)
{
    QFile sourceFile(path);
    qint64 fileSize = sourceFile.size();
    const int bufferSize = 2048;

    if (sourceFile.open(QIODevice::ReadOnly)) {
        char buffer[bufferSize];
        int bytesRead;

        QCryptographicHash hash(QCryptographicHash::Md5);

        if ((bytesRead = sourceFile.read(buffer, qMin(fileSize, bufferSize))) > 0) {
            hash.addData(buffer, bytesRead);
        }

        sourceFile.close();
        return QString(hash.result().toHex());
    }

    return QString();
}

bool Data::verify(){
    QFileInfo info(_filePath);
    if (info.isFile()) {
        return _md5 == getFileMd5(_filePath);
    }
    return false;
}

Data::Data(const BriefInfo& info,const QString& path){
    QFileInfo f(path);
    assert(f.isFile());
    _duration = info.totalMs;
    _fileName = f.fileName();
    _filePath = path;
    _md5 = getFileMd5(path);
    _lastTime = 0;
    dir.mkpath(info.mediaType);
    if(!info.qimage.isNull())
        info.qimage.save(dir.filePath(info.mediaType+"/"+_fileName+".jpg"));
}

void Data::setLastTime(qint64 time){
    _lastTime = time>=0?(_duration>time?time:_duration):0;
}

QJsonObject Data::toJson(){
    QJsonObject json;
    json.insert("fileName",_fileName);
    json.insert("filePath",_filePath);
    json.insert("md5",_md5);
    json.insert("duration",_duration);
    json.insert("lastTime",_lastTime);
    return json;
}

Data::Data(const QJsonObject& json)
{
    _fileName = json["fileName"].toString();
    _filePath = json["filePath"].toString();
    _md5 = json["md5"].toString();
    _duration = json["duration"].toInteger();
    _lastTime = json["lastTime"].toInteger();
}


