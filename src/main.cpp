#include <QApplication>
#include <QQmlApplicationEngine>

#include "AppWorker.hpp"

int main(int argc, char *argv[]) {
    //qputenv("QT_QPA_PLATFORM", "xcb");

    QApplication app(argc, argv);

    QQmlApplicationEngine qmlEngine;

    // Пути поиска
    qDebug() << "Import paths:" << qmlEngine.importPathList();

    // Регистрируем тип движка как Singleton
    qmlRegisterSingletonType<AppWorker>("Engine", 1, 0, "Engine", AppWorker::create);
    //qmlRegisterUncreatableMetaObject(Engine::staticMetaObject, "Engine", 1, 0, "StdTypesEngine", "");
    // Регистрируем модели данных
    qmlRegisterType<Engine::OrderbookSideModel>("Engine.Components", 1, 0, "OrderbookSideModel");
    // Регистрируем стандартные типы движка
    qmlRegisterType<Engine::Ticker>("Engine.Components", 1, 0, "ticker");
    qmlRegisterType<Engine::Orderbook>("Engine.Components", 1, 0, "orderbook");
    qmlRegisterType<Engine::Kline>("Engine.Components", 1, 0, "kline");

    QObject::connect(&qmlEngine, &QQmlApplicationEngine::objectCreationFailed, &app, []() {
        QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    qmlEngine.loadFromModule("MainApplication", "Main");

    return app.exec();
}
