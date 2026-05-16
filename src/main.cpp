#include <QApplication>
#include <QQmlContext>
#include <QQmlApplicationEngine>

#include "Engine/App/AppWorker.hpp"

int main(int argc, char *argv[]) {
    //qputenv("QT_QPA_PLATFORM", "xcb");

    QApplication app(argc, argv);

    QQmlApplicationEngine qmlEngine;

    // Регестрируем тип AppWorker в контексте QML как ApplicationEngine в модуле MyApp
    qmlRegisterType<AppWorker>("MyApp", 1, 0, "ApplicationEngine");
    // Регестрируем пространство имен Engine в контексте QML в модуле Engine
    qmlRegisterUncreatableMetaObject(Engine::staticMetaObject, "Engine", 1, 0, "Engine", "Access to enums only");

    qmlRegisterUncreatableType<Engine::stTicker>("Engine", 1, 0, "stTicker", "Cannot create in QML");
    qmlRegisterUncreatableType<Engine::stOrderBooks>("Engine", 1, 0, "stOrderBooks", "Cannot create in QML");

    const QUrl url("qrc:/QML/Main.qml");

    QObject::connect(&qmlEngine, &QQmlApplicationEngine::objectCreationFailed, &app, []() {
        QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    qmlEngine.load(url);

    return app.exec();
}
