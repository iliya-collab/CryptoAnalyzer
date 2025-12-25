#include "StyleManager.hpp"

QString StyleManager::loadStyleSheet(const QString &path) {
    QFile file(":/styles/" + path);
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << WAR << "Cannot load style sheet:" << path;
        return QString();
    }
    
    QTextStream stream(&file);
    QString style = stream.readAll();
    file.close();
    
    return style;
}