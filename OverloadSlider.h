#ifndef OVERLOADSLIDER_H
#define OVERLOADSLIDER_H

#include <QObject>
#include <QMouseEvent>
#include <QSlider>

class OverloadSlider : public QSlider
{
    Q_OBJECT
public:
    explicit OverloadSlider(QWidget *parent = nullptr);
    void mousePressEvent(QMouseEvent *e);

signals:

};

#endif // OVERLOADSLIDER_H
