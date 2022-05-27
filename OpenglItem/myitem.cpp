#include "myitem.h"
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

//void MyItem::timerEvent(QTimerEvent* event)
//{
//    Q_UNUSED(event);
//    if(_isPlay)
//        update();
//}

void MyItem::bind(Controller* obj){
    if(obj==nullptr){
        return;
    }
    connect(obj,&Controller::update,this,&MyItem::onUpdate);
}


void MyItem::onUpdate(std::shared_ptr<YUVData> data){
//    if(!_isPlay)
//        return;
    this->data = data;
    update();
}

