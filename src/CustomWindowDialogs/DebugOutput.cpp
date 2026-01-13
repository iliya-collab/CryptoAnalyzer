#include "CustomWindowDialogs/DebugOutput.hpp"

#include <QDateTime>
#include <QApplication>
#include <QScrollBar>

DebugOutput* DebugOutput::m_instance = nullptr;
QMutex DebugOutput::m_mutex;

DebugOutput::DebugOutput(QObject *parent) : QObject(parent), m_textEdit(nullptr), m_defaultHandler(nullptr) {}

DebugOutput* DebugOutput::instance() {
    QMutexLocker locker(&m_mutex);
    if (!m_instance)
        m_instance = new DebugOutput();
    return m_instance;
}

void DebugOutput::setTextEdit(QTextEdit *textEdit) {
    m_textEdit = textEdit;
    
    connect(this, &DebugOutput::newMessage, this, [this](const QString &message, int type) {
        if (!m_textEdit) return;
        
        QString color;
        switch (type) {
            case QtDebugMsg: color = "gray"; break;
            case QtInfoMsg: color = "blue"; break;
            case QtWarningMsg: color = "orange"; break;
            case QtCriticalMsg: color = "red"; break;
            case QtFatalMsg: color = "darkred"; break;
            default: color = "black"; break;
        }
        
        QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss");
        QString html = QString("<span style='color:%1'>[%2] %3</span><br>")
                          .arg(color, timestamp, message.toHtmlEscaped());
        
        m_textEdit->append(html);
        
        QScrollBar *scrollBar = m_textEdit->verticalScrollBar();
        scrollBar->setValue(scrollBar->maximum());
    });
}

void DebugOutput::redirectQtMessages() {
    m_defaultHandler = qInstallMessageHandler(messageHandler);
}

void DebugOutput::restoreQtMessages() {
    qInstallMessageHandler(m_defaultHandler);
}

void DebugOutput::write(const QString &message, QtMsgType type) {
    emit newMessage(message, type);
}

void DebugOutput::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    Q_UNUSED(context);
    
    QString formattedMsg = qFormatLogMessage(type, context, msg);
    
    instance()->write(formattedMsg, type);
    
    //fprintf(stderr, "%s\n", qPrintable(formattedMsg));
}