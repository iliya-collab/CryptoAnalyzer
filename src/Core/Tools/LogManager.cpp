#include "LogManager.hpp"
#include <QDebug>
#include <cstdio>

namespace Core::Tools {

    LogManager& LogManager::instance() {
        static LogManager instance;
        return instance;
    }

    LogManager::LogManager() {
        qInstallMessageHandler(LogManager::messageHandler);
    }

    void LogManager::setDebugEnabled(bool enabled) { m_debugEnabled = enabled; }
    void LogManager::setInfoEnabled(bool enabled) { m_infoEnabled = enabled; }
    void LogManager::setWarningEnabled(bool enabled) { m_warningEnabled = enabled; }

    bool LogManager::isDebugEnabled() const { return m_debugEnabled; }
    bool LogManager::isInfoEnabled() const { return m_infoEnabled; }
    bool LogManager::isWarningEnabled() const { return m_warningEnabled; }

    void LogManager::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
        LogManager& manager = LogManager::instance();

        if (type == QtDebugMsg && !manager.m_debugEnabled)
            return;

        if (type == QtInfoMsg && !manager.m_infoEnabled)
            return;

        if (type == QtWarningMsg && !manager.m_warningEnabled)
            return;

        QByteArray localMsg = msg.toLocal8Bit();

        switch (type) {
        case QtDebugMsg:
            fprintf(stderr, "Debug: %s \n", localMsg.constData());
            break;
        case QtInfoMsg:
            fprintf(stderr, "Info: %s \n", localMsg.constData());
            break;
        case QtWarningMsg:
            fprintf(stderr, "Warning: %s \n", localMsg.constData());
            break;
        case QtCriticalMsg:
            fprintf(stderr, "Critical: %s \n", localMsg.constData());
            break;
        case QtFatalMsg:
            fprintf(stderr, "Fatal: %s \n", localMsg.constData());
            abort();
        }

        fflush(stderr);
    }

}