#pragma once

#include <QObject>
#include <QThread>
#include <atomic>
#include <memory>

#include "Engine/App/AppEngine.hpp"
#include "Engine/App/AppEngineLoader.hpp"
#include "Engine/App/AppEngineСonfiguration.hpp"

class AppWorker : public QObject {
    Q_OBJECT

    Q_PROPERTY(QVariantList tradeList READ getTradeList NOTIFY tradeListChanged)
    Q_PROPERTY(QVariantList APIKeys READ getAPIKeys NOTIFY APIKeysChanged)

private:

    std::unique_ptr<Engine::AppEngineСonfiguration> m_config;
    std::unique_ptr<Engine::AppEngineLoader> m_loader;
    std::unique_ptr<Engine::AppEngine> m_engine;

    QThread* m_workerThread;
    Engine::WebSocketEndpoints m_pendingEndpoint;

    QString m_loadTrade;
    QVariantList m_lstTrades;
    Engine::API m_curAPI;

    std::atomic<bool> m_hasLoaded{false};
    std::atomic<bool> m_hasStarted{false};

    void setupLoaderConnections();
    void setupEngineConnections();
    void setupConnections();

public:

    AppWorker(QObject* parent = nullptr);
    ~AppWorker();

    // ------ Методы доступные в контексте QML ------
    // Запускает движок
    Q_INVOKABLE void run(Engine::WebSocketEndpoints endpoint);
    Q_INVOKABLE void setAPI(const Engine::API& api);
    // Запускает trade по выбранной паре pair
    Q_INVOKABLE void addTrade(const QString& pair);
    // Отбрасывает все пары, которые не заданы в фильтре
    Q_INVOKABLE void filter(const QString& pair, bool on = true);
    // Сохраняет api ключ под названием name в файле конфигурации
    Q_INVOKABLE void saveAPI(const QString& name, const QString& apiKey, const QString& secretKey, bool isTestnet);

    // ------ Методы для работы с свойствами в контексте QML ------
    // Используется при чтении свойства tradeList
    QVariantList getTradeList() const;
    // Используется при чтении свойства APIKeys
    QVariantList getAPIKeys() const;

signals:

    // ------ Сигналы для уведомления об измении свойства ------
    // Изменение свойства tradeList
    void tradeListChanged();
    // Изменение свойства APIKeys
    void APIKeysChanged();

    void loadingProgress(const QString& step, int current, int total);
    void errorOccurred(const QString& error);
    void loadingFinished(bool success);
    void engineStarted();
    void tickerUpdated(const Engine::stTicker& ticker);

};