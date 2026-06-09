#include <QApplication>
#include <QQmlApplicationEngine>

#include "Engine/App/AppWorker.hpp"

int main(int argc, char *argv[]) {
    //qputenv("QT_QPA_PLATFORM", "xcb");

    QApplication app(argc, argv);

    QQmlApplicationEngine qmlEngine;

    // Пути поиска
    qDebug() << "Import paths:" << qmlEngine.importPathList();

    qmlRegisterSingletonType<AppWorker>("Engine", 1, 0, "Engine", AppWorker::create);
    qmlRegisterUncreatableMetaObject(Engine::staticMetaObject, "Engine", 1, 0, "StdTypesEngine", "Access to enums only");
    qmlRegisterType<OrderBookSideModel>("Engine.Components", 1, 0, "OrderBookSideModel");
    qmlRegisterType<Engine::stTicker>("Engine.Components", 1, 0, "stTicker");
    qmlRegisterType<Engine::stOrderBook>("Engine.Components", 1, 0, "stOrderBook");
    qmlRegisterType<Engine::stKline>("Engine.Components", 1, 0, "stKline");

    QObject::connect(&qmlEngine, &QQmlApplicationEngine::objectCreationFailed, &app, []() {
        QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    qmlEngine.loadFromModule("MainApplication", "Main");

    return app.exec();
}
