#include "myrender.h"
#include <QOpenGLFramebufferObject>
#include <QQuickWindow>
#include <QQuickOpenGLUtils>
#include <QOpenGLPixelTransferOptions>
#include <QFile>
#define ver 0
#define tex 1

MyRender::MyRender(QObject *parent)
    : QObject{parent}
{
    init();
}

void MyRender::init(){
    //初始化opengl上下文
    initializeOpenGLFunctions();

    //初始化坐标数据
    mVertices << QVector2D(-1, -1) << QVector2D(1, -1) << QVector2D(-1, 1) << QVector2D(1, 1);
    mTexcoords << QVector2D(0, 0) << QVector2D(1, 0) << QVector2D(0, 1) << QVector2D(1, 1);

    //初始化shader
    mProgram.addCacheableShaderFromSourceCode(QOpenGLShader::Vertex,
                                              "attribute highp vec4 vertex;"
                                              "attribute highp vec2 texCoord;"
                                              "varying vec2 texCoordOut;"
                                              "void main(void) {"
                                              "gl_Position = vertex;"
                                              "texCoordOut = texCoord;}");
    mProgram.addCacheableShaderFromSourceCode(QOpenGLShader::Fragment,
                                              "varying vec2 texCoordOut;"
                                              "uniform sampler2D tex_y;"
                                              "uniform sampler2D tex_u;"
                                              "uniform sampler2D tex_v;"
                                              "void main(void){"
                                              "vec3 yuv;"
                                              "vec3 rgb;"
                                              "yuv.x = texture2D(tex_y, texCoordOut).r;"
                                              "yuv.y = texture2D(tex_u, texCoordOut).r - 0.5;"
                                              "yuv.z = texture2D(tex_v, texCoordOut).r - 0.5;"
                                              "rgb = mat3( 1,       1,         1,"
                                              "0,       -0.39465,  2.03211,"
                                              "1.13983, -0.58060,  0) * yuv;"
                                              "gl_FragColor = vec4(rgb, 1);}");
    mProgram.bindAttributeLocation("vertex", ver);
    mProgram.bindAttributeLocation("texCoord", tex);

    //初始化贴图
    glGenTextures(3, texArray);
    for(int i = 0;i<3;i++){
        glBindTexture(GL_TEXTURE_2D, texArray[i]);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    mProgram.link();
    mProgram.bind();
}

void MyRender::render(){
    //清空缓冲区
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    mProgram.bind();

    //调整glViewport的大小使其比例不变
    float scale = (windowWidth/data->yWidth<windowHeight/data->yHeight)?(windowWidth/data->yWidth):(windowHeight/data->yHeight);
    float offsetX = (windowWidth-scale*data->yWidth>0)?(windowWidth-scale*data->yWidth):0;
    float offsetY = (windowHeight-scale*data->yHeight>0)?(windowHeight-scale*data->yHeight):0;
    glViewport(offsetX/2,offsetY/2,data->yWidth*scale,data->yHeight*scale);


    //导入顶点坐标数据
    mProgram.enableAttributeArray(ver);
    mProgram.setAttributeArray(ver,mVertices.constData());

    //导入uv坐标数据
    mProgram.enableAttributeArray(tex);
    mProgram.setAttributeArray(tex,mTexcoords.constData());

    //设置着色器中的贴图
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,texArray[0]);
    glTexImage2D ( GL_TEXTURE_2D, 0, GL_LUMINANCE, data->yWidth, data->yHeight, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data->Y);
    mProgram.setUniformValue("tex_y", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,texArray[1]);
    glTexImage2D ( GL_TEXTURE_2D, 0, GL_LUMINANCE, data->uWidth, data->uHeight, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data->U);
    mProgram.setUniformValue("tex_u", 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D,texArray[2]);
    glTexImage2D ( GL_TEXTURE_2D, 0, GL_LUMINANCE, data->vWidth, data->vHeight, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data->V);
    mProgram.setUniformValue("tex_v", 2);


    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    mProgram.disableAttributeArray(ver);
    mProgram.disableAttributeArray(tex);
    mProgram.release();

    //重置opengl上下文
    QQuickOpenGLUtils::resetOpenGLState();
}

QOpenGLFramebufferObject* MyRender::createFramebufferObject(const QSize& size){
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(4);
    windowHeight =size.height();
    windowWidth = size.width();
    glViewport(0, 0, size.width(), size.height());
    return new QOpenGLFramebufferObject(size, format);
}

void MyRender::synchronize(QQuickFramebufferObject* item){
    updateData();
}

//更新YUV数据
void MyRender::updateData(){
    if(fp==nullptr){
        fp = fopen("C:\\Users\\17866\\Desktop\\qttemp\\build-MyPlayer-Desktop_Qt_6_3_0_MSVC2019_64bit-Debug\\out240x128.yuv", "rb");
        if (fp==nullptr)
        {
            qDebug()<<("out240x128.yuv file open failed!");
        }
    }
    if(fp!=nullptr){
        if (feof(fp))
        {
            fseek(fp, 0, SEEK_SET);
        }
        const int width = 240;
        const int height = 128;
        this->data = std::make_shared<YUVData>();
        data->yHeight = height;data->yWidth = width;
        data->uHeight = height/2;data->uWidth = width/2;
        data->vHeight = height/2;data->vWidth = width/2;
        fread(data->Y, 1, width * height, fp);
        fread(data->U, 1, width * height / 4, fp);
        fread(data->V, 1, width * height / 4, fp);
    }
}
