#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <cstdlib>

#include "facade/InspectorFacade.h"
#include "renderers/Native3DGraphItem.h"

int main(int argc, char *argv[]) {
    // 1. Wayland display crash-ஐத் தடுத்து X11/XCB முறையைத் தேர்ந்தெடுக்கிறது
    qputenv("QT_QPA_PLATFORM", "xcb");

    QGuiApplication app(argc, argv);

    // Register Native C++ OpenGL 3D Graph Item for QML
    qmlRegisterType<Native3DGraphItem>("ROSInspector.Renderers", 1, 0, "Native3DGraphItem");

    QQmlApplicationEngine engine;

    InspectorFacade inspectorFacade;
    engine.rootContext()->setContextProperty("inspectorFacade", &inspectorFacade);

    const QUrl url(QStringLiteral("qrc:/qt/qml/ROSInspector/ui/main.qml"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}