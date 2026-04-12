#pragma once

#include <QObject>
#include <QThread>
#include <atomic>
#include <memory>

#include "Engine/App/AppEngine.hpp"
#include "Engine/App/AppEngineLoader.hpp"

class AppWorker : public QObject {
    Q_OBJECT
public:
    AppWorker(QObject* parent = nullptr);
    ~AppWorker();

    Q_INVOKABLE void startTrade(Engine::TypesTrade trade);

signals:
    void loadingProgress(const QString& step, int current, int total);
    void errorOccurred(const QString& error);
    void loadingFinished(bool success);
    void engineStarted();

private:
    std::unique_ptr<Engine::AppEngineLoader> m_loader;
    std::unique_ptr<Engine::AppEngine> m_engine;
    QThread* m_workerThread;
    QUrl m_pendingUrl;

    std::atomic<bool> m_hasLoaded{false};
    std::atomic<bool> m_hasStarted{false};

    void setupLoaderConnections();
    void setupEngineConnections();
    void setupConnections();

};