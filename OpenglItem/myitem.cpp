﻿#include "myitem.h"
#include "myrender.h"
#include "controller.h"

MyItem::MyItem()
{
    setFlag(QQuickItem::ItemHasContents,true);
    //startTimer(1000 / 24);
}

QQuickFramebufferObject::Renderer* MyItem::createRenderer()const{
    return new MyRender;
}

void MyItem::timerEvent(QTimerEvent* event)
{
    Q_UNUSED(event);
    if(_isPlay)
        update();
}

void MyItem::bind(Controller* obj){
    connect(obj,&Controller::update,this,&MyItem::onUpdate);
}

