#include "controller.h"


Controller::Controller(QObject *parent)
    : QObject{parent}
{

}

void Controller::stopVideo(){
    emit pause();
}
void Controller::stopAudio(){
    emit pause();
}

void Controller::exit(){
    //退出播放前先记录播放历史
    manager.recordVideo(_time);
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

void Controller::startPlayAudio(int index){
    //先进行检查
    State s = manager.checkAudio(index);
    if(s == State::Normal){
        //设置当前视频
        manager.setCurAudio(index);
        //发送对应的信号
        emit playMedia(manager.getAudio(index).filePath());
    }
    else if(s==State::Error){
        emit fileError(manager.getAudio(index).filePath());
    }
    else if(s==State::Miss){
        emit fileMiss(manager.getAudio(index).filePath());
    }
}

void Controller::startPlayVideo(int index){
    //先进行检查
    State s = manager.checkVideo(index);
    if(s == State::Normal){
        //记录之前的播放历史
        manager.recordVideo(_time);
        //设置当前视频
        manager.setCurVideo(index);
        //发送对应的信号
        emit playMedia(manager.getVideo(index).filePath());
    }
    else if(s==State::Error){
        emit fileError(manager.getVideo(index).filePath());
    }
    else if(s==State::Miss){
        emit fileMiss(manager.getVideo(index).filePath());
    }
}

void Controller::playNextAudio(){
    int index = manager.nextAudio();
    startPlayAudio(index);
}

void Controller::playPreAudio(){
    int index = manager.preAudio();
    startPlayAudio(index);
}

void Controller::playNextVideo(){
    int index = manager.nextVideo();
    startPlayAudio(index);
}

void Controller::playPreVideo(){
    int index = manager.preVideo();
    startPlayAudio(index);
}

void Controller::importVideo(const QList<QString>& list){
    State s;
    for(auto& item:list){
        s = manager.importVideo(item);
        if(s==State::Error){
            emit fileError(item);
        }
        else if(s==State::Miss){
            emit fileMiss(item);
        }
    }
}

void Controller::importAudio(const QList<QString>& list){
    State s;
    for(auto& item:list){
        s = manager.importAudio(item);
        if(s==State::Error){
            emit fileError(item);
        }
        else if(s==State::Miss){
            emit fileMiss(item);
        }
    }
}

void Controller::deleteAudio(int index){
    manager.deleteAudio(index);
}

void Controller::deleteVideo(int index){
    manager.deleteVideo(index);
}

