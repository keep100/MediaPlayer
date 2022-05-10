#include "widget.h"
#include "./ui_widget.h"
#include <QFileDialog>
#include <QDebug>
#include "XDemuxThread.h"
#include <QMessageBox>
#include <iostream>
XDemuxThread dt;
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    dt.Start();
    startTimer(40);
}

Widget::~Widget()
{
    qDebug() << "Widget::~Widget()";
    dt.Close();
    delete ui;
}
void Widget::OpenFile()
{
    //选择文件
    QString name = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择视频文件"));
    if (name.isEmpty())return;
    this->setWindowTitle(name);
    if (dt.isPause == 1)
    {
        PlayOrPause();
    }
    if (!dt.Open(name.toLocal8Bit(), ui->openGLWidget))
    {
        QMessageBox::information(0, "error", "open file failed!");
        return;
    }
    SetPause(dt.isPause);

}
void Widget::SliderPress()
{
    isSliderPress = 1;
}
void Widget::SliderRelease()
{
    isSliderPress = 0;
    double pos = 0.0;
    pos = ui->horizontalSlider->value() / (double)ui->horizontalSlider->maximum();
    dt.Seek(pos);
}
void Widget::SetPause(bool isPause)
{
    if (isPause)
            ui->isPlay->setText("播放");
        else
            ui->isPlay->setText("暂停");
}
void Widget::PlayOrPause()
{
    std::cout << "slot\n\n\n\n\n\n\n\n\n\n\n\n\n";
    bool isPause = !dt.isPause;
    dt.SetPause(isPause);
    SetPause(isPause);

}
//窗口尺寸变化
void Widget::resizeEvent(QResizeEvent *e)
{
    ui->horizontalSlider->move(50, this->height() - 100);
    ui->horizontalSlider->resize(this->width() - 100, ui->horizontalSlider->height());

    ui->openFile->move(100,this->height() - 150);
    ui->isPlay->move(ui->openFile->x() + ui->openFile->width() + 10, ui->openFile->y());
    ui->openGLWidget->resize(this->size());
}
//定时器 滑动条显示
void Widget::timerEvent(QTimerEvent *e)
{
    if (isSliderPress)
        return ;
    long long total = dt.totalMs;
    if (total > 0)
    {
        double pos = (double)dt.pts /(double) total;
        int v = ui->horizontalSlider->maximum() * pos;
        ui->horizontalSlider->setValue(v);
    }
}
//双击全屏
void Widget::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (isFullScreen())
        this->showNormal();
    else
        this->showFullScreen();
}
