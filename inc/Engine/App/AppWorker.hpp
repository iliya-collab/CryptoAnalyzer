#pragma once

#include <QObject>
#include <QThread>
#include <atomic>

#include "Engine/App/AppEngine.hpp"
#include "Engine/App/AppEngineLoader.hpp"

class AppWorker : public QObject {
    Q_OBJECT
public:
    AppWorker(QObject* parent = nullptr);
    ~AppWorker();

signals:
    void loadingProgress(int current, int total);
    void errorOccurred(const QString& error);
    void loadingFinished(bool success);

private:
    Engine::AppEngineLoader* m_loader = nullptr;
    Engine::AppEngine* m_engine = nullptr;
    QThread* m_thread = nullptr;

    std::atomic<bool> m_hasLoaded{false};
    std::atomic<bool> m_hasStarted{false};

    void setupConnections();
;
};