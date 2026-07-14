#include <QApplication>
#include <QQmlApplicationEngine>
#include "AppCore.hpp"

int main(int argc, char *argv[]) {
    //qputenv("QT_QPA_PLATFORM", "xcb");

    QApplication app(argc, argv);

    QQmlApplicationEngine qmlEngine;

    // Пути поиска
    qDebug() << "Import paths:" << qmlEngine.importPathList();

    QObject::connect(&qmlEngine, &QQmlApplicationEngine::objectCreationFailed, &app, []() {
        QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    qmlEngine.loadFromModule("MainApplication", "Main");

    return app.exec();
}
