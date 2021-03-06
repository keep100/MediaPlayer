#ifndef MYITEM_H
#define MYITEM_H
#include "myrender.h"
#include <QQuickFramebufferObject>
#include <QSGNode>
#include "src/Controller/controller.h"


class MyItem : public QQuickFramebufferObject
{
    Q_OBJECT
    Q_PROPERTY(bool isPlay READ isPlay WRITE setIsPlay NOTIFY isPlayChanged)
public:
    MyItem();
//    void timerEvent(QTimerEvent* event) override;
    Renderer* createRenderer() const override;
    bool isPlay(){return _isPlay;}
    void setIsPlay(bool state){_isPlay = state;emit isPlayChanged();}
    Q_INVOKABLE void bind(Controller* obj);
    void onUpdate(std::shared_ptr<YUVData> data);
public:
    std::shared_ptr<YUVData> data = nullptr;

private:
    bool _isPlay = true;


signals:
    void isPlayChanged();

};

#endif // MYITEM_H
