#include "datamanager.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <random>

DataManager::DataManager(QObject *parent)
    : QObject{parent}
{
    dir.setPath("./Data");
    readData();
}

void DataManager::setMode(PlayMode::mode m){
    mode = m;
    if(m==PlayMode::mode::Random){
        std::random_device rd;
        std::mt19937 rng(rd());
        QList<int>& list = _isAudio?_audioOrder:_videoOrder;
        //对order列表进行洗牌操作
        std::shuffle(list.begin(),list.end(),rng);
        //把当前正在播放的设置为列表头
        int pos = list.indexOf(curIndex);
        if(pos>=0){
            list.swapItemsAt(pos,0);
            curOrder = 0;
        }
    }
}

void DataManager::setCur(int index,bool isAudio){
    curIndex = index;
    _isAudio = isAudio;

    //如果是随机播放就调整curOrder的值
    if(mode==PlayMode::Random){
        curOrder = isAudio?_audioOrder.indexOf(index):_videoOrder.indexOf(index);
    }

    if(isAudio){
        emit curAudioChanged();
    }
    else{
        emit curVideoChanged();
    }
}


void DataManager::readData(){
    //导入音频列表数据
    QFileInfo info(dir.filePath("audio.json"));
    if(info.isFile()){
        QFile file(info.filePath());
        if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
            QJsonParseError parseJsonErr;
            QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseJsonErr);
            file.close();
            if (!(parseJsonErr.error == QJsonParseError::NoError)) {
                //文件出错则进行删除
                QFile::remove(info.filePath());
            }
            else{
                QJsonArray jsonArray = document.array();
                for(auto item:jsonArray){
                    _audioList.append(item.toObject());
                    _audioOrder.append(_audioOrder.size());
                }
            }
        }
    }

    //导入视频列表数据
    info.setFile(dir.filePath("video.json"));
    if(info.isFile()){
        QFile file(info.filePath());
        if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
            QJsonParseError parseJsonErr;
            QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseJsonErr);
            file.close();
            if (!(parseJsonErr.error == QJsonParseError::NoError)) {
                //文件出错则进行删除
                QFile::remove(info.filePath());
            }
            else{
                QJsonArray jsonArray = document.array();
                for(auto item:jsonArray){
                    _videoList.append(item.toObject());
                    _videoOrder.append(_videoOrder.size());
                }
            }
        }
    }

}

void DataManager::writeData(){
    if(!dir.exists()){
        dir.setPath(".");
        if(!dir.mkpath("Data")){
            return;
        }
        dir.setPath("./Data");
    }

    //将音频列表写入json文件中
    QFile audioFile(dir.filePath("audio.josn"));
    if(audioFile.open(QFile::WriteOnly)){
        QJsonArray array;
        for(auto& item:_audioList){
            array.append(item.toJson());
        }
        QJsonDocument doc(array);
        audioFile.write(doc.toJson());
        audioFile.close();
    }

    //将视频列表写入json文件中
    QFile videoFile(dir.filePath("video.josn"));
    if(audioFile.open(QFile::WriteOnly)){
        QJsonArray array;
        for(auto& item:_videoList){
            array.append(item.toJson());
        }
        QJsonDocument doc(array);
        videoFile.write(doc.toJson());
        videoFile.close();
    }

}

DataManager::~DataManager(){
    writeData();
}

void DataManager::deleteData(int index,bool isAudio){
    QList<Data>& list = isAudio?_audioList:_videoList;
    QList<int>& order = _isAudio?_audioOrder:_videoOrder;
    if(index<list.size()&&index>=0){
        list.removeAt(index);

        //同步修改顺序列表中的元素
        for(int i=0;i<order.size();i++){
            order[i] -= order[i]>index?1:0;
        }
        order.removeOne(index);

        if(isAudio)
            emit audioListChanged();
        else
            emit videoListChanged();
    }
}

State DataManager::importData(const QString& path,bool isAudio){
    QFileInfo f(path);
    if(f.isFile()){
        QString type = isAudio?"audio":"video";
        QList<Data>& list = isAudio?_audioList:_videoList;
        QList<int>& order = _isAudio?_audioOrder:_videoOrder;
        qDebug()<<6666;
        //对文件进行解析
        auto info = XMediaManager::getBriefInfo(path.toLocal8Bit());
        qDebug()<<7778;
        if(info.mediaType!=type){
            return State::Error;
        }
        //判断文件是否已经存在于列表中
        int index = 0;
        for(int i=0;i<list.size();i++){
            if(list[i].filePath() == path)
                break;
        }
        //如果不存在则在列表尾部添加
        if(index==list.size()){
            list.append(Data(info,f));
            order.append(index);
        }
        else{
            //如果是已经存在则直接修改
            list[index] = Data(info,f);
        }
        if(isAudio)
            emit audioListChanged();
        else
            emit videoListChanged();
        return State::Normal;
    }
    else{
        return State::Miss;
    }
}

State DataManager::check(int index,bool isAudio){
    QList<Data>& list = isAudio?_audioList:_videoList;
    if(index<list.size()&&index>=0){
        return list[index].verify();
    }
    return State::Error;
}

Data DataManager::getData(int index,bool isAudio){
    QList<Data>& list = isAudio?_audioList:_videoList;
    if(index<list.size()&&index>=0){
        return list[index];
    }
    return Data();
}

QVariantList DataManager::videoList(){
    QVariantList list;
    for(auto& item:_videoList){
        list<<QVariant::fromValue(item);
    }
    return list;
}

QVariantList DataManager::audioList(){
    QVariantList list;
    for(int i = 0;i<_audioList.size();i++){
        _audioList[i].setIndex(i);
        list<<QVariant::fromValue(_audioList[i]);
    }
    return list;
}

int DataManager::next(){
    QList<Data>& list = _isAudio?_audioList:_videoList;
    QList<int>& order = _isAudio?_audioOrder:_videoOrder;

    //如果不是随机播放就直接返回下一个
    if(mode!=PlayMode::Random){
        if(curIndex>=list.size()-1){
            curIndex = 0;
        }
        else{
            curIndex++;
        }
        return curIndex;
    }
    else{
        if(curOrder>=order.size()-1){
            curOrder = 0;
        }
        else{
            curOrder++;
        }
        return order[curOrder];
    }
}

int DataManager::pre(){
    QList<Data>& list = _isAudio?_audioList:_videoList;
    QList<int>& order = _isAudio?_audioOrder:_videoOrder;

    //如果不是随机播放就直接返回下一个
    if(mode!=PlayMode::Random){
        if(curIndex<=0){
            curIndex = list.size()-1;
        }
        else{
            curIndex--;
        }
        return curIndex;
    }
    else{
        if(curOrder<=0){
            curOrder = order.size()-1;
        }
        else{
            curOrder--;
        }
        return order[curOrder];
    }

}

Data DataManager::curAudio(){
    if(_isAudio)
        return _audioList[curIndex];
    return Data();
}

Data DataManager::curVideo(){
    if(!_isAudio)
        return _videoList[curIndex];
    return Data();
}

void DataManager::recordVideo(long long time){
    if(!_isAudio){
        _videoList[curIndex].setLastTime(time);
    }
}



