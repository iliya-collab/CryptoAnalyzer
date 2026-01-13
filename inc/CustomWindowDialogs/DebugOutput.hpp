#pragma once

#include <QObject>
#include <QTextEdit>
#include <QMutex>

class DebugOutput : public QObject
{
    Q_OBJECT
public:
    static DebugOutput* instance();
    
    void setTextEdit(QTextEdit *textEdit);
    void redirectQtMessages();
    void restoreQtMessages();
    
    void write(const QString &message, QtMsgType type = QtDebugMsg);
    
signals:
    void newMessage(const QString &message, int type);
    
private:
    DebugOutput(QObject *parent = nullptr);
    static DebugOutput* m_instance;
    static QMutex m_mutex;
    
    QTextEdit *m_textEdit;
    QtMessageHandler m_defaultHandler;
    
    static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
};