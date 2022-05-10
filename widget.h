#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    //定时器 滑动条显示
    void timerEvent(QTimerEvent *e);

    //窗口尺寸变化
    void resizeEvent(QResizeEvent *e);


    //双击全屏
    void mouseDoubleClickEvent(QMouseEvent *e);
    //暂停
    void SetPause(bool isPause);
    bool isFirst = 0;
public slots:
    void OpenFile();
    void PlayOrPause();
    void SliderPress();
    void SliderRelease();

private:
    bool isSliderPress = false;
    Ui::Widget *ui;
};
#endif // WIDGET_H
