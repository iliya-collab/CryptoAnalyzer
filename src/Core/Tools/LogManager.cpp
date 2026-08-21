#include "LogManager.hpp"
#include <QDebug>
#include <QTime>
#include <cstdio>

namespace Core::Tools {

    LogManager& LogManager::instance()
    {
        static LogManager instance;
        return instance;
    }

    LogManager::LogManager()
    {
        qInstallMessageHandler(LogManager::messageHandler);
    }

    void LogManager::setDebugEnabled(bool enabled)
    {
        QMutexLocker locker(&m_mutex);
        m_debugEnabled = enabled;
    }

    void LogManager::setInfoEnabled(bool enabled)
    {
        QMutexLocker locker(&m_mutex);
        m_infoEnabled = enabled;
    }

    void LogManager::setWarningEnabled(bool enabled)
    {
        QMutexLocker locker(&m_mutex);
        m_warningEnabled = enabled;
    }

    void LogManager::setLogFile(const QString &name)
    {
        QMutexLocker locker(&m_mutex);

        if (m_logFile.isOpen())
            m_logFile.close();

        m_logFile.setFileName(name);

        m_isLogFile = m_logFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
    }

    void LogManager::setLogFileEnabled(bool enabled)
    {
        QMutexLocker locker(&m_mutex);
        m_isLogFile = enabled;
    }

    void LogManager::setStdLogEnabled(bool enanbled)
    {
        QMutexLocker locker(&m_mutex);
        m_isStdLog = enanbled;
    }

    bool LogManager::isDebugEnabled() const
    {
        QMutexLocker locker(&m_mutex);
        return m_debugEnabled;
    }

    bool LogManager::isInfoEnabled() const
    {
        QMutexLocker locker(&m_mutex);
        return m_infoEnabled;
    }

    bool LogManager::isWarningEnabled() const
    {
        QMutexLocker locker(&m_mutex);
        return m_warningEnabled;
    }

    bool LogManager::isLogFileEnabled() const
    {
        QMutexLocker locker(&m_mutex);
        return m_isLogFile;
    }

    bool LogManager::isStdLogEnabled() const
    {
        QMutexLocker locker(&m_mutex);
        return m_isStdLog;
    }

    void LogManager::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
    {
        LogManager& manager = LogManager::instance();

        QMutexLocker locker(&manager.m_mutex);

        if (type == QtDebugMsg && !manager.m_debugEnabled)
            return;

        if (type == QtInfoMsg && !manager.m_infoEnabled)
            return;

        if (type == QtWarningMsg && !manager.m_warningEnabled)
            return;

        QByteArray localMsg = msg.toLocal8Bit();
        QString timeStamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
        QByteArray timeBytes = timeStamp.toLocal8Bit();

        if (manager.m_isLogFile && manager.m_logFile.isOpen())
        {
            QTextStream out(&manager.m_logFile);
            out << "[" << timeStamp << "] " << msg << "\n";
            out.flush();
        }

        if (!manager.m_isStdLog)
            return;

        switch (type)
        {
        case QtDebugMsg:
            fprintf(stderr, "[%s] Debug: %s\n", timeBytes.constData(), localMsg.constData());
            break;
        case QtInfoMsg:
            fprintf(stderr, "[%s] Info: %s\n", timeBytes.constData(), localMsg.constData());
            break;
        case QtWarningMsg:
            fprintf(stderr, "[%s] Warning: %s\n", timeBytes.constData(), localMsg.constData());
            break;
        case QtCriticalMsg:
            fprintf(stderr, "[%s] Critical: %s\n", timeBytes.constData(), localMsg.constData());
            break;
        case QtFatalMsg:
            fprintf(stderr, "[%s] Fatal: %s\n", timeBytes.constData(), localMsg.constData());
            abort();
        }

        fflush(stderr);
    }

}