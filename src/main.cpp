#include <QApplication>
#include <QQmlApplicationEngine>

#include "Engine/App/AppWorker.hpp"
#include "Engine/Tools/OrderBookModel.hpp"

int main(int argc, char *argv[]) {
    //qputenv("QT_QPA_PLATFORM", "xcb");

    QApplication app(argc, argv);

    QQmlApplicationEngine qmlEngine;

    // Выводим пути поиска
    qDebug() << "Import paths:" << qmlEngine.importPathList();

    qmlRegisterSingletonType<AppWorker>("Engine", 1, 0, "Engine", AppWorker::create);
    qmlRegisterUncreatableMetaObject(Engine::staticMetaObject, "Engine", 1, 0, "StdTypesEngine", "Access to enums only");

    qmlRegisterType<OrderBookModel>("EngineComponents", 1, 0, "OrderBookModel");
    qmlRegisterType<Engine::stTicker>("EngineComponents", 1, 0, "stTicker");
    qmlRegisterType<Engine::stOrderBook>("EngineComponents", 1, 0, "stOrderBook");

    QObject::connect(&qmlEngine, &QQmlApplicationEngine::objectCreationFailed, &app, []() {
        QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    qmlEngine.loadFromModule("MainApplication", "Main");

    return app.exec();
}
