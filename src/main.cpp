#include "mainwindow.hpp"
#include "CustomWindowDialogs/DebugMonitor.hpp"

int main(int argc, char *argv[])
{
    qputenv("QT_QPA_PLATFORM", "xcb");

    QApplication app(argc, argv);
    
    //DebugMonitor debug;
    //debug.show();

    MainWindow win;
    win.show();
    
    return app.exec();
}
