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
    
     if (m_textEdit) {
        // Важные настройки
        m_textEdit->setAcceptRichText(false);  // Используем plain text
        m_textEdit->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
        m_textEdit->setLineWrapMode(QTextEdit::WidgetWidth);
        
        // Установить моноширинный шрифт
        QFont font("Monospace", 9);
        font.setStyleHint(QFont::TypeWriter);
        m_textEdit->setFont(font);
        
        // Настроить табуляцию
        m_textEdit->setTabStopDistance(40);  // 40 пикселей
        
        // Остальные настройки
        m_textEdit->setReadOnly(true);
        m_textEdit->setUndoRedoEnabled(false);
    }

    connect(this, &DebugOutput::newMessage, this, [this](const QString &message, int type) {
        /*if (!m_textEdit) return;
        
        QString color;
        switch (type) {
            case QtDebugMsg: color = "gray"; break;
            case QtInfoMsg: color = "blue"; break;
            case QtWarningMsg: color = "orange"; break;
            case QtCriticalMsg: color = "red"; break;
            case QtFatalMsg: color = "darkred"; break;
            default: color = "black"; break;
        }
        
        QString htmlMessage = message.toHtmlEscaped()
            .replace("\n", "<br>")
            .replace("\t", "&nbsp;&nbsp;&nbsp;&nbsp;");

        QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss");
        QString html = QString("<span style='color:%1'>[%2] %3</span><br>")
                          .arg(color, timestamp, htmlMessage);
        
        //QString html = QString("<span style='color:%1'> %2</span><br>").arg(color, message.toHtmlEscaped());
        m_textEdit->append(html);
        
        QScrollBar *scrollBar = m_textEdit->verticalScrollBar();
        scrollBar->setValue(scrollBar->maximum());*/
        if (!m_textEdit) return;

        QColor color;
        switch (type) {
            case QtDebugMsg: color = Qt::gray; break;
            case QtInfoMsg: color = Qt::blue; break;
            case QtWarningMsg: color = QColor(255, 165, 0); break;
            case QtCriticalMsg: color = Qt::red; break;
            case QtFatalMsg: color = Qt::darkRed; break;
            default: color = Qt::black; break;
        }

        QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss");
        QString formattedMessage = QString("[%1] %2\n").arg(timestamp, message);

        QTextCharFormat format;
        format.setForeground(color);

        QTextCursor cursor(m_textEdit->document());
        cursor.movePosition(QTextCursor::End);
        cursor.insertText(formattedMessage, format);

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
}