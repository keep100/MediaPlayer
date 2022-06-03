#include "datamanager.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <random>

QStringList getFileListUnderDir(const QString &dirPath)
{
    QStringList fileList;
    QDir dir(dirPath);
    QFileInfoList fileInfoList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot | QDir::Dirs);
    foreach (auto fileInfo, fileInfoList) {
        if(fileInfo.isDir())
        {

            getFileListUnderDir(fileInfo.absoluteFilePath());
        }

        if(fileInfo.isFile())
        {
            fileList.append(fileInfo.absoluteFilePath());
            //qDebug() << __FUNCTION__ << __LINE__ << "  : " << fileInfo.absoluteFilePath();
        }
    }
    return fileList;
}

DataManager::DataManager(QObject *parent)
    : QObject{parent}
{
    dir.setPath("./Data");
    readData();
    _audioOrder.ini(_audioList.size());
    _videoOrder.ini(_videoList.size());
}

void DataManager::setMode(PlayMode::mode m)
{
    mode = m;
    if(m != PlayMode::mode::Random)
    {
        _audioOrder.reset();
        _videoOrder.reset();
    }
    else
    {
        if(_isAudio)
        {
            _audioOrder.setOredr(curIndex);
        }
        else
        {
            _videoOrder.setOredr(curIndex);
        }
    }
}

void DataManager::setCur(int index, bool isAudio)
{
    curIndex = index;
    _isAudio = isAudio;

    //如果是随机播放就调整curOrder的值
    if(mode == PlayMode::Random)
    {
        if(_isAudio)
        {
            _audioOrder.setOredr(curIndex);
        }
        else
        {
            _videoOrder.setOredr(curIndex);
        }
    }

    if(isAudio)
    {
        emit curAudioChanged();
    }
    else
    {
        emit curVideoChanged();
    }
}


void DataManager::readData()
{
    //导入音频列表数据
    QFileInfo info(dir.filePath("audio.json"));
    if(info.isFile())
    {
        QFile file(info.filePath());
        if(file.open(QFile::ReadOnly))
        {
            QJsonParseError parseJsonErr;
            QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseJsonErr);
            file.close();
            if (!(parseJsonErr.error == QJsonParseError::NoError))
            {
                //文件出错则进行删除
                QFile::remove(info.filePath());
            }
            else
            {
                QJsonArray jsonArray = document.array();
                for(auto item : jsonArray)
                {
                    _audioList.append(item.toObject());
                    info.setFile(_audioList.back().filePath());
                    if(!info.isFile())
                    {
                        _audioList.removeLast();
                    }
                }
            }
        }
    }

    //导入视频列表数据
    info.setFile(dir.filePath("video.json"));
    if(info.isFile())
    {
        QFile file(info.filePath());
        if(file.open(QFile::ReadOnly))
        {
            QJsonParseError parseJsonErr;
            QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseJsonErr);
            file.close();
            if (!(parseJsonErr.error == QJsonParseError::NoError))
            {
                //文件出错则进行删除
                QFile::remove(info.filePath());
            }
            else
            {
                QJsonArray jsonArray = document.array();
                for(auto item : jsonArray)
                {
                    _videoList.append(item.toObject());
                    info.setFile(_videoList.back().filePath());
                    if(!info.isFile())
                    {
                        _videoList.removeLast();
                    }
                }

            }
        }
    }

    //导入用户数据
    info.setFile(dir.filePath("userInfo.json"));
    if(info.isFile())
    {
        QFile file(info.filePath());
        if(file.open(QFile::ReadOnly))
        {
            QJsonParseError parseJsonErr;
            QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseJsonErr);
            file.close();
            if (!(parseJsonErr.error == QJsonParseError::NoError))
            {
                //文件出错则进行删除
                QFile::remove(info.filePath());
            }
            else
            {
                _userInfo.init(document.object());
            }
        }
    }
}

void DataManager::writeData()
{
    if(!dir.exists())
    {
        dir.setPath(".");
        if(!dir.mkpath("Data"))
        {
            return;
        }
        dir.setPath("./Data");
    }


    //将音频列表写入json文件中
    QFile audioFile(dir.filePath("audio.json"));
    if(audioFile.open(QFile::WriteOnly))
    {
        //获取缩略图文件list，用于删除已经不存在的缩略图
        auto list = getFileListUnderDir(dir.filePath("thumpnail/audio"));

        QJsonArray array;
        for(auto &item : _audioList)
        {
            array.append(item.toJson());
            list.removeAll(item.imgPath());
        }
        for(auto& path:list){
            QFile::remove(path);
        }
        QJsonDocument doc(array);
        audioFile.write(doc.toJson());
        audioFile.close();
    }

    //将视频列表写入json文件中
    QFile videoFile(dir.filePath("video.json"));
    if(videoFile.open(QFile::WriteOnly))
    {
        auto list = getFileListUnderDir(dir.filePath("thumpnail/video"));
        QJsonArray array;
        for(auto &item : _videoList)
        {
            array.append(item.toJson());
            list.removeAll(item.imgPath());
        }
        for(auto& path:list){
            qDebug()<<path;
            QFile::remove(path);
        }
        QJsonDocument doc(array);
        videoFile.write(doc.toJson());
        videoFile.close();
    }

    //写入用户数据
    QFile userFile(dir.filePath("userInfo.json"));
    if(userFile.open(QFile::WriteOnly))
    {
        QJsonObject json = _userInfo.toJson();
        QJsonDocument doc(json);
        userFile.write(doc.toJson());
        userFile.close();
    }
}

DataManager::~DataManager()
{
    writeData();
}

void DataManager::deleteData(int index, bool isAudio)
{
    //如果删除当前正在播放的则直接返回
    if(index == curIndex && isAudio == _isAudio)
    {
        return;
    }
    QList<Data> &list = isAudio ? _audioList : _videoList;
    RandomList &order = isAudio ? _audioOrder : _videoOrder;
    if(index < list.size() && index >= 0)
    {
        //删除缩略图
        list.removeAt(index);
        order.moveOrder(index);     //同步修改顺序列表
        //如果修改的是当前正在播放的列表则修改对应的索引
        if(index < curIndex && isAudio == _isAudio)
        {
            curIndex --;
        }
        if(isAudio)
        {
            emit audioListChanged();
        }
        else
        {
            emit videoListChanged();
        }
    }
}

State DataManager::importData(const QString &path, bool isAudio)
{
    QFileInfo f(path);
    if(f.isFile())
    {
        QString type = isAudio ? "audio" : "video";
        QList<Data> &list = isAudio ? _audioList : _videoList;
        RandomList &order = isAudio ? _audioOrder : _videoOrder;
        //对文件进行解析
        auto info = XMediaManager::getBriefInfo(path);
        if(info.mediaType != type)
        {
            return State::Error;
        }
        //判断文件是否已经存在于列表中
        int index = 0;
        for(; index < list.size(); index++)
        {
            if(list[index].filePath() == f.filePath())
            {
                break;
            }
        }
        //如果不存在则在列表尾部添加
        if(index == list.size())
        {
            order.addOrder();        //同步修改顺序列表
            list.append(Data(info, f));
        }
        else
        {
            //如果是已经存在则直接修改
            list[index] = Data(info, f);
        }
        if(isAudio)
        {
            emit audioListChanged();
        }
        else
        {
            emit videoListChanged();
        }
        return State::Normal;
    }
    else
    {
        return State::Miss;
    }
}

State DataManager::check(int index, bool isAudio)
{
    QList<Data> &list = isAudio ? _audioList : _videoList;
    if(index < list.size() && index >= 0)
    {
        return list[index].verify();
    }
    return State::Error;
}

Data DataManager::getData(int index, bool isAudio)
{
    QList<Data> &list = isAudio ? _audioList : _videoList;
    if(index < list.size() && index >= 0)
    {
        return list[index];
    }
    return Data();
}

QVariantList DataManager::videoList()
{
    QVariantList list;
    for(int i = 0; i < _videoList.size(); i++)
    {
        _videoList[i].setIndex(i);
        list << QVariant::fromValue(_videoList[i]);
    }
    return list;
}

QVariantList DataManager::audioList()
{
    QVariantList list;
    for(int i = 0; i < _audioList.size(); i++)
    {
        _audioList[i].setIndex(i);
        list << QVariant::fromValue(_audioList[i]);
    }
    return list;
}

int DataManager::next()
{
    QList<Data> &list = _isAudio ? _audioList : _videoList;
    RandomList &order = _isAudio ? _audioOrder : _videoOrder;

    //如果不是随机播放就直接返回下一个
    if(mode != PlayMode::Random)
    {
        if(curIndex >= list.size() - 1)
        {
            return 0;
        }
        return curIndex + 1;

    }
    else
    {
        return order.next();
    }
}

int DataManager::pre()
{
    QList<Data> &list = _isAudio ? _audioList : _videoList;
    RandomList &order = _isAudio ? _audioOrder : _videoOrder;

    //如果不是随机播放就直接返回上一个
    if(mode != PlayMode::Random)
    {
        if(curIndex <= 0)
        {
            return list.size() - 1;
        }
        return curIndex - 1;
    }
    else
    {
        return order.pre();
    }

}

Data DataManager::curAudio()
{
    if(_isAudio && curIndex <= _audioList.size() && curIndex >= 0)
    {
        return _audioList[curIndex];
    }
    return Data();
}

Data DataManager::curVideo()
{
    if(!_isAudio && curIndex <= _videoList.size() && curIndex >= 0)
    {
        return _videoList[curIndex];
    }
    return Data();
}

void DataManager::recordVideo(long long time)
{
    if(!_isAudio && curIndex <= _videoList.size() && curIndex >= 0)
    {
        _videoList[curIndex].setLastTime(time);
    }
}
void DataManager::reset()
{
    curIndex = -1;
    _audioOrder.reset();
    _videoOrder.reset();
    emit curAudioChanged();
    emit curVideoChanged();
}

void DataManager::setUserInfo(QString path, float ratio)
{
    _userInfo.setBckPath(path);
    _userInfo.setBlurRatio(ratio);
    emit userInfoChanged();
}



