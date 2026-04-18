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

    qmlEngine.load(QUrl(QStringLiteral("qrc:/QML/main.qml")));
    if (qmlEngine.rootObjects().isEmpty()) {
        qDebug().noquote() << "Runtime error qml engine";
        return -1;
    }

    return app.exec();
}
