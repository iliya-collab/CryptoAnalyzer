#include <QApplication>
#include <QQmlApplicationEngine>

#include "Engine/App/AppWorker.hpp"

int main(int argc, char *argv[]) {
    //qputenv("QT_QPA_PLATFORM", "xcb");

    QApplication app(argc, argv);

    QQmlApplicationEngine qmlEngine;

    // Выводим пути поиска
    qDebug() << "Import paths:" << qmlEngine.importPathList();

    qmlRegisterSingletonType<AppWorker>("Engine", 1, 0, "Engine", AppWorker::create);
    qmlRegisterUncreatableMetaObject(Engine::staticMetaObject, "Engine", 1, 0, "StdTypesEngine", "Access to enums only");
    qmlRegisterUncreatableType<Engine::stTicker>("Engine", 1, 0, "stTicker", "Cannot create in QML");
    qmlRegisterUncreatableType<Engine::stOrderBook>("Engine", 1, 0, "stOrderBook", "Cannot create in QML");

    QObject::connect(&qmlEngine, &QQmlApplicationEngine::objectCreationFailed, &app, []() {
        QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    qmlEngine.loadFromModule("MainApplication", "Main");

    return app.exec();
}
