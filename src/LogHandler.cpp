#include "LogHandler.hpp"

#include <QFile>
#include <QDate>

bool LogHandler::enable = false;
const char* LogHandler::file_log = "log.txt";

void LogHandler::writeLog(const char* msg) {
    if (!enable)
        return;

    QFile file(file_log);

    if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        // Добавляем временную метку
        QString timestamp = QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss] ");

        file.write(QString("%1 %2\n").arg(timestamp).arg(msg).toUtf8());
        file.close();
    }
}

void LogHandler::clearLog() {
    if (!enable)
        return;

    QFile file(file_log);

    if (file.open(QIODevice::WriteOnly))
        file.close();
}

void LogHandler::setEnable(bool _enable) {
    enable = _enable;
}
