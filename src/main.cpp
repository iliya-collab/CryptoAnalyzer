#include <QApplication>
#include <QQmlContext>
#include <QQmlApplicationEngine>

#include "Engine/App/AppWorker.hpp"

int main(int argc, char *argv[]) {
    //qputenv("QT_QPA_PLATFORM", "xcb");

    QApplication app(argc, argv);

    AppWorker worker;

    QQmlApplicationEngine qmlEngine;

    // Регестрируем свойство appEngine в контексте QML
    qmlEngine.rootContext()->setContextProperty("appEngine", &worker);
    // Регестрируем пространство имен Engine в контексте QML а качестве модуля Engine
    qmlRegisterUncreatableMetaObject(Engine::staticMetaObject, "Engine", 1, 0, "Engine", "Access to enums only");

    qmlEngine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (qmlEngine.rootObjects().isEmpty()) {
        qDebug().noquote() << "Runtime error qml engine";
        return -1;
    }

    return app.exec();
}
