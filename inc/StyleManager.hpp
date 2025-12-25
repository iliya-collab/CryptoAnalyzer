#ifndef _STYLE_
#define _STYLE_

#include <QString>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QApplication>

#ifndef OPERATION_STATUS
#define OPERATION_STATUS

#define OK  "\033[32m✔\033[0m"
#define ERR "\033[31m✗\033[0m"
#define WAR "\033[33m⚠\033[0m"

#endif

class StyleManager {

public:
    static QString loadStyleSheet(const QString &path);

};

#endif