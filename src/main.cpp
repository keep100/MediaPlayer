#include "Video/XVideoThread.h"
#include "Demux/XDemuxThread.h"
#include "utility/cursorposprovider.h"
#include "XMediaManager.h"
#include "OpenglItem/myitem.h"
#include "Controller/controller.h"

//#define QT_NO_DEBUG_OUTPUT      //屏蔽debug输出

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);
    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);
    QQmlApplicationEngine engine;
    Controller::init(engine);
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

    XMediaManager mm;
    qmlRegisterType<MyItem>("MyItem", 1, 0, "MyItem");
    engine.load(url);
    return app.exec();
}
