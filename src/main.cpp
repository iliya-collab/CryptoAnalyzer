#include "mainwindow.hpp"

int main(int argc, char *argv[])
{
    qputenv("QT_QPA_PLATFORM", "xcb");

    QApplication app(argc, argv);
    
    MainWindow win;
    win.show();
    
    return app.exec();
}
