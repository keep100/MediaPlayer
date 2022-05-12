#ifndef XVIDEOWIDGET_H
#define XVIDEOWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QWidget>
#include <QDebug>
#include <QOpenGLShader>
#include <mutex>
#include "IVideoCall.h"

struct AVFrame;
class XVideoWidget : public QOpenGLWidget, protected QOpenGLFunctions, public IVideoCall
{
    Q_OBJECT
public:
    XVideoWidget(QWidget *parent);
    ~XVideoWidget();


    virtual void Init(int width, int height);

    //不管成功与否都释放frame空间
    virtual void Repaint(AVFrame *frame);
protected:
    //刷新显示
    void paintGL();
    //初始化
    void initializeGL();
    //尺寸变化
    void resizeGL(int height, int width);
private:

    std::mutex mux;
    QOpenGLShaderProgram program;

    //shader中yuv变量地址
    GLuint unis[3] = {0};
    //openg的 texture地址
    GLuint texs[3] = {0};

    //材质内存空间
    unsigned char *datas[3] = {0};

    int width = 240;
    int height = 128;
};

#endif // XVIDEOWIDGET_H
