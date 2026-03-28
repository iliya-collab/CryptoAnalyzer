#include <QApplication>
#include <QQmlContext>
#include <QQmlApplicationEngine>

#include "Engine/App/AppWorker.hpp"

int main(int argc, char *argv[]) {
    //qputenv("QT_QPA_PLATFORM", "xcb");

    QApplication app(argc, argv);

    AppWorker worker;

    QQmlApplicationEngine qmlEngine;

    qmlEngine.rootContext()->setContextProperty("appEngine", &worker);

    qmlEngine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (qmlEngine.rootObjects().isEmpty()) {
        qDebug().noquote() << "Runtime error qml engine";
        return -1;
    }

    return app.exec();
}
