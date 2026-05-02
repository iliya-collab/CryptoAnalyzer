#pragma once

#include <QObject>
#include <QThread>
#include <atomic>
#include <memory>

#include "Engine/App/AppEngine.hpp"
#include "Engine/App/AppEngineLoader.hpp"

class AppWorker : public QObject {
    Q_OBJECT

    Q_PROPERTY(QVariantList tradeList READ tradeList NOTIFY tradeListChanged)
    Q_PROPERTY(Engine::TypeTrade trade READ trade WRITE setTrade NOTIFY tradeChanged)

public:
    AppWorker(QObject* parent = nullptr);
    ~AppWorker();

    Q_INVOKABLE void startEngine();
    Q_INVOKABLE void startTrade(const QString& pair);

    QVariantList tradeList() const;

    Engine::TypeTrade trade() const { return m_trade; }
    void setTrade(Engine::TypeTrade trade) { m_trade = trade; }

signals:
    void tradeChanged();
    void tradeListChanged();

    void loadingProgress(const QString& step, int current, int total);
    void errorOccurred(const QString& error);
    void loadingFinished(bool success);

    void engineStarted();

    void tickerUpdated(const Engine::stTicker& ticker);

private:
    std::unique_ptr<Engine::AppEngineLoader> m_loader;
    std::unique_ptr<Engine::AppEngine> m_engine;
    QThread* m_workerThread;
    QUrl m_pendingUrl;

    Engine::TypeTrade m_trade;
    QVariantList m_lstTrades;

    std::atomic<bool> m_hasLoaded{false};
    std::atomic<bool> m_hasStarted{false};

    void setupLoaderConnections();
    void setupEngineConnections();
    void setupConnections();

};