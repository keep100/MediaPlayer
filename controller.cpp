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
void Controller::startPlayVideo(int index){
    emit playVideo(index);
}
void Controller::startPlayAudio(int index){
    emit playAudio(index);
}
void Controller::importAudioData(QList<QString> list){
    emit importAudio(list);
}
void Controller::deleteAudioData(qint64 index){
    emit deleteAudio(index);
}
void Controller::importVideoData(QList<QString> list){
    emit importVideo(list);
}
void Controller::deleteVideoData(qint64 index){
    emit deleteVideo(index);
}
void Controller::playNextVideo(){
    emit nextVideo();
}
void Controller::playPreVideo(){
    emit preVideo();
}
void Controller::playNextAudio(){
    emit nextAudio();
}
void Controller::playPreAudio(){
    emit preAudio();
}

void Controller::exit(){
    emit exitPlay();
}

PlayMode::mode Controller::mode(){
    return _mode;
}
float Controller::voice(){
    return _voice;
}
float Controller::playSpeed(){
    return _playSpeed;
}

void Controller::setMode(PlayMode::mode m){
    _mode = m;
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

void Controller::searchAudio(QString s){
    emit queryAudio(s);
}

void Controller::searchVideo(QString s){
    emit queryVideo(s);
}
