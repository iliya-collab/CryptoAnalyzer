#include <QApplication>
#include <QQmlApplicationEngine>
#include "AppCore.hpp"

int main(int argc, char *argv[]) {
    //qputenv("QT_QPA_PLATFORM", "xcb");

    QApplication app(argc, argv);

    QQmlApplicationEngine qmlEngine;

    // Пути поиска
    qDebug() << "Import paths:" << qmlEngine.importPathList();

    // Регистрируем модели данных
    qmlRegisterType<Engine::OrderbookSideModel>("Engine.Components", 1, 0, "OrderbookSideModel");
    // Регистрируем стандартные типы движка
    qmlRegisterType<Engine::Ticker>("Engine.Components", 1, 0, "Ticker");
    qmlRegisterType<Engine::Orderbook>("Engine.Components", 1, 0, "Orderbook");
    qmlRegisterType<Engine::Kline>("Engine.Components", 1, 0, "Kline");
    // Регистрируем тип движка как Singleton
    qmlRegisterSingletonType<AppCore>("Engine", 1, 0, "Engine", AppCore::create);

    QObject::connect(&qmlEngine, &QQmlApplicationEngine::objectCreationFailed, &app, []() {
        QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    qmlEngine.loadFromModule("MainApplication", "Main");

    return app.exec();
}
