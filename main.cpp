#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "imageprovider.h"
#include "XVideoThread.h"
#include "XDemuxThread.h"
#include "cursorposprovider.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
//    ShowImage *myImage = new ShowImage();
//    engine.rootContext()->setContextProperty("MyImage", myImage);
//    engine.addImageProvider(QLatin1String("Imgs"), myImage->m_pImgProvider);
//    XDemuxThread *thread = new XDemuxThread();
//    engine.rootContext()->setContextProperty("MyImage", thread->vt->showImage);
//    engine.addImageProvider(QLatin1String("Imgs"), thread->vt->showImage->m_pImgProvider);
    CursorPosProvider mousePosProvider;
    engine.rootContext()->setContextProperty("mousePosition", &mousePosProvider);


    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject * obj, const QUrl & objUrl)
    {
        if (!obj && url == objUrl)
        {
            QCoreApplication::exit(-1);
        }
    }, Qt::QueuedConnection);
    engine.load(url);

//    QImage image;
//    bool flag = image.load("C:/Users/16321/Pictures/Saved Pictures/15.jpg");
//    qDebug() << flag;
//    myImage->sendimage(image);
//    thread->Open("C:/Users/16321/Documents/Tencent Files/1632195270/FileRecv/Packet Tracer的基本使用.mp4",
//                 nullptr);

    return app.exec();
}
