#include "Managers/RestartManager.hpp"

#include <QApplication>
#include <QDebug>
#include <QProcess>
#include <QTimer>

bool RestartManager::m_restartRequested = false;

void RestartManager::requestRestart() {
    m_restartRequested = true;

    // Немедленно начинаем процесс перезапуска
    QTimer::singleShot(0, []() {
        performRestart();
    });
}

void RestartManager::performRestart() {
    if (!m_restartRequested)
        return;

    qDebug() << "Performing application restart...";

    QApplication::closeAllWindows();

    QCoreApplication::processEvents();

    QString appPath = QApplication::applicationFilePath();
    QStringList args = QApplication::arguments();

    if (!args.isEmpty())
        args.removeFirst();

    // Важно: запускаем через startDetached и сразу выходим
    if (QProcess::startDetached(appPath, args)) {
        // Даем время новому процессу запуститься
        QTimer::singleShot(100, qApp, &QCoreApplication::quit);
    } else {
        qWarning() << "Failed to restart application";
        m_restartRequested = false;
    }
}
