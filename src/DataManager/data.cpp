#include "data.h"
#include<QCryptographicHash>


QDir Data::dir = QDir("./Data/thumpnail");

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

State Data::verify(){
    QFileInfo info(_filePath);
    if (info.isFile()) {
        return _md5 == getFileMd5(_filePath)?State::Normal:State::Error;
    }
    return State::Miss;
}

Data::Data(const BriefInfo& info,const QFileInfo& f){
    _isAudio = info.mediaType=="audio"?true:false;
    _duration = info.totalMs;
    _fileName = info.title!=""?info.title:f.fileName();
    _filePath = f.filePath();
    _md5 = getFileMd5(_filePath);
    _lastTime = 0;
    _album = info.album;
    _artist = info.artist;
    _aBitRate = info.aBitRate;
    _vBitRate = info.vBitRate;
    _channels = info.channels;
    _codecId = info.codecId;
    _width = info.width;
    _height = info.height;
    dir.mkpath(info.mediaType);
    if(!info.img.isNull()){
        QCryptographicHash hash(QCryptographicHash::Md5);
        hash.addData(_filePath.toLocal8Bit());
        _imgPath = dir.absoluteFilePath(info.mediaType+"/"+hash.result().toHex()+".jpg");
        info.img.save(_imgPath);
    }

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
    json.insert("isAudio",_isAudio);
    json.insert("imgPath",_imgPath);
    json.insert("album",_album);
    json.insert("artist",_artist);
    json.insert("codecId",_codecId);
    json.insert("aBitRate",_aBitRate);
    json.insert("vBitRate",_vBitRate);
    json.insert("channels",_channels);
    json.insert("width",_width);
    json.insert("height",_height);
    return json;
}

Data::Data(const QJsonObject& json)
{
    _fileName = json["fileName"].toString();
    _filePath = json["filePath"].toString();
    _md5 = json["md5"].toString();
    _duration = json["duration"].toInteger();
    _lastTime = json["lastTime"].toInteger();
    _isAudio = json["isAudio"].toBool();
    _imgPath = json["imgPath"].toString();
    _album = json["album"].toString();
    _artist = json["artist"].toString();
    _channels = json["channels"].toInt();
    _codecId = json["codecId"].toString();
    _aBitRate = json["aBitRate"].toInt();
    _vBitRate = json["vBitRate"].toInt();
    _width = json["width"].toInt();
    _height = json["height"].toInt();
}


