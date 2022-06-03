#ifndef MYRENDER_H
#define MYRENDER_H

#include <QObject>
#include <QQuickFramebufferObject>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include "utility/yuvdata.h"

class MyRender :public QQuickFramebufferObject::Renderer,protected QOpenGLFunctions
{
public:
    MyRender();
    void render() override;
    QOpenGLFramebufferObject* createFramebufferObject(const QSize& size) override;
    void synchronize(QQuickFramebufferObject*) override;
    void init();
    ~MyRender(){
        if(fp!=nullptr){
            fclose(fp);
        }
    }
private:
    QOpenGLShaderProgram mProgram;  //shader
    QVector<QVector2D> mVertices;   //顶点数组
    QVector<QVector2D> mTexcoords;  //纹理坐标数组
    unsigned int texArray[3] = {0}; //贴图数组，每一项对应一个贴图
    std::shared_ptr<YUVData> data = nullptr;          //存放YUV数据
    float windowHeight,windowWidth; //存放窗口大小（用float是为了除法运算）
    FILE* fp = nullptr;
private:
    void updateData();              //更新YUV数据

signals:

};

#endif // MYRENDER_H
