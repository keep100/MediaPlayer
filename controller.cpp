#include "controller.h"


Controller::Controller(QObject *parent)
    : QObject{parent}
{

}

void Controller::stop(){
    emit pause();
}


void Controller::exit(){
    //退出播放前先记录播放历史
    manager.recordVideo(_time);
    manager.reset();
    emit exitPlay();
}

void Controller::setMode(PlayMode::mode m){
    _mode = m;
    manager.setMode(m);
    emit modeChanged(m);
}

void Controller::setPlaySpeed(float s){
    _playSpeed = s;
    emit playSpeedChanged(s);
}

void Controller::setVoice(float v){
    _voice = v;
    emit voiceChanged(v);
}

void Controller::setTime(int t){
    _time = t;
    emit skipTime(t);
}

void Controller::startPlay(int index,bool isAudio){
    //先进行检查
    State s = manager.check(index,isAudio);
    if(s == State::Normal){
        //设置当前视频
        manager.setCur(index,isAudio);
        //发送对应的信号
        emit playMedia(manager.getData(index,isAudio).filePath());
    }
    else if(s==State::Error){
        emit fileError(manager.getData(index,isAudio));
    }
    else if(s==State::Miss){
        emit fileMiss(manager.getData(index,isAudio));
    }
}


void Controller::playNext(bool isAudio){
    startPlay( manager.next(),isAudio);
}

void Controller::playPre(bool isAudio){
    startPlay(manager.pre(),isAudio);
}


void Controller::importData(const QList<QString>& list,bool isAudio){
    State s;
    for(auto& item:list){
        s = manager.importData(item,isAudio);
        if(s==State::Error){
            emit fileImportFail(item);
        }
        else if(s==State::Miss){
            emit fileImportFail(item);
        }
    }
}

void Controller::deleteData(int index,bool isAudio){
    manager.deleteData(index,isAudio);
}


