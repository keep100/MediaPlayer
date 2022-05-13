#ifndef MYRENDER_H
#define MYRENDER_H

#include <QObject>
#include <QQuickFramebufferObject>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

const static int cBufferSize = 1024 * 768;
struct YUVData
{
    unsigned char Y[cBufferSize];
    unsigned char U[cBufferSize];
    unsigned char V[cBufferSize];
    int yHeight=0,yWidth=0;
    int uWidth=0,uHeight=0;
    int vHeight=0,vWidth=0;
};

class MyRender : public QObject,public QQuickFramebufferObject::Renderer,protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit MyRender(QObject *parent = nullptr);
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
    std::shared_ptr<YUVData> data;  //存放YUV数据
    float windowHeight,windowWidth; //存放窗口大小（用float是为了除法运算）
    FILE* fp = nullptr;
private:
    void updateData();              //更新YUV数据

signals:

};

#endif // MYRENDER_H
