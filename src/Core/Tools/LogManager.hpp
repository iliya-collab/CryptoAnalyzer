#pragma once
#include <QObject>
#include <QMessageLogContext>
#include <QString>
#include <QFile>
#include <QMutex>

namespace Core::Tools {

    class LogManager {
    public:

        static LogManager& instance();

        void setDebugEnabled(bool enabled);
        void setInfoEnabled(bool enabled);
        void setWarningEnabled(bool enabled);

        void setLogFile(const QString& name);
        void setLogFileEnabled(bool enabled);
        void setStdLogEnabled(bool enanbled);

        bool isDebugEnabled() const;
        bool isInfoEnabled() const;
        bool isWarningEnabled() const;
        bool isLogFileEnabled() const;
        bool isStdLogEnabled() const;

    private:

        LogManager();
        ~LogManager() = default;

        LogManager(const LogManager&) = delete;
        LogManager& operator= (const LogManager&) = delete;

        static void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);

        bool m_debugEnabled = true;
        bool m_infoEnabled = true;
        bool m_warningEnabled = true;

        bool m_isLogFile = false;
        bool m_isStdLog = true;

        QFile m_logFile{};
        mutable QMutex m_mutex;
    };

}
