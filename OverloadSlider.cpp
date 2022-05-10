#include "OverloadSlider.h"

OverloadSlider::OverloadSlider(QWidget *parent)
    : QSlider{parent}
{

}
void OverloadSlider::mousePressEvent(QMouseEvent *e)
{
    double pos = e->pos().x() / (double)width();
    setValue(pos * this->maximum());
    //原有事件处理
    QSlider::mousePressEvent(e);
    //QSlider::sliderReleased();
}
