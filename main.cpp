#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
//#include "until/imageprovider.h"
#include "XVideoThread.h"
#include "XDemuxThread.h"
#include "until/cursorposprovider.h"
#include "XMediaManager.h"
#include<QDebug>
#include "OpenglItem/myitem.h"
#include <QQuickWindow>


int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);
    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);
    QQmlApplicationEngine engine;
//    ShowImage *myImage = new ShowImage();
//    engine.rootContext()->setContextProperty("MyImage", myImage);
//    engine.addImageProvider(QLatin1String("Imgs"), myImage->m_pImgProvider);
//    XDemuxThread *thread = new XDemuxThread();
//    engine.rootContext()->setContextProperty("MyImage", thread->vt->showImage);
//    engine.addImageProvider(QLatin1String("Imgs"), thread->vt->showImage->m_pImgProvider);
    CursorPosProvider mousePosProvider;
    engine.rootContext()->setContextProperty("mousePosition", &mousePosProvider);

    QString fileName = "D:\\Videos\\[大体积]Evangelion.3.0+1.01.Thrice.Upon.a.Time.2021.中文字幕.WEBrip.AAC.1080p.x264-VINEnc.mp4";
//    QString fileName = "‪C:\\Users\\16409\\Videos\\rick.and.morty.s05\\Rick.and.Morty.S05E01.Mort.Dinner.Rick.Andre.1080p.AMZN.WEBRip.DDP5.1.x264-NTb[eztv.re].mkv";
//    QString fileName = "C:\\Users\\16409\\Videos\\电影\\生化危机1.Resident Evil.2002.BluRay.HEVC.1080p.AC3.2Audios.中英特效.mp4";
    BriefInfo briefInfo = XMediaManager::getBriefInfo(fileName.toLocal8Bit());
    qDebug() << briefInfo.mediaType;
    briefInfo.qimage.save("C:\\Users\\16409\\Desktop\\oneFrame.jpg");

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject * obj, const QUrl & objUrl)
    {
        if (!obj && url == objUrl)
        {
            QCoreApplication::exit(-1);
        }
    }, Qt::QueuedConnection);

    qmlRegisterType<MyItem>("MyItem", 1, 0, "MyItem");
    engine.load(url);

//    QImage image;
//    bool flag = image.load("C:/Users/16321/Pictures/Saved Pictures/15.jpg");
//    qDebug() << flag;
//    myImage->sendimage(image);
//    thread->Open("C:/Users/16321/Documents/Tencent Files/1632195270/FileRecv/Packet Tracer的基本使用.mp4",
//                 nullptr);

    return app.exec();
}
