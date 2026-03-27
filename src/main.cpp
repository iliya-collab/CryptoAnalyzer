#include <QApplication>
#include <QQmlApplicationEngine>

#include "Engine/App/AppWorker.hpp"

int main(int argc, char *argv[]) {
    qputenv("QT_QPA_PLATFORM", "xcb");

    QApplication app(argc, argv);

    AppWorker worker;

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
