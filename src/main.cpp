#include "mainwindow.hpp"

#include "Settings.hpp"
#include "StyleManager.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Если конфиг файл существует и он валидный, то считываем конфиг с него
    if (!Settings::readAllConfig())
        qDebug() << WAR << Settings::getLastError();

    MainWindow win(StyleManager::loadStyleSheet("_main.qss"));
    win.show();

    return app.exec();
}
