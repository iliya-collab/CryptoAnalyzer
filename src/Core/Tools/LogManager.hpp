#pragma once
#include <QObject>
#include <QMessageLogContext>
#include <QString>

namespace Core::Tools {

    class LogManager {
    public:

        static LogManager& instance();

        void setDebugEnabled(bool enabled);
        void setInfoEnabled(bool enabled);
        void setWarningEnabled(bool enabled);

        bool isDebugEnabled() const;
        bool isInfoEnabled() const;
        bool isWarningEnabled() const;

    private:

        LogManager();
        ~LogManager() = default;

        LogManager(const LogManager&) = delete;
        LogManager& operator= (const LogManager&) = delete;

        static void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);

        bool m_debugEnabled = true;
        bool m_infoEnabled = true;
        bool m_warningEnabled = true;
    };

}
