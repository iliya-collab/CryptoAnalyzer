#pragma once

#include <QQmlContext>

#include <QObject>
#include <QThread>
#include <atomic>
#include <memory>

#include "Engine/App/AppEngine.hpp"
#include "Engine/App/AppEngineLoader.hpp"

class AppWorker : public QObject {
    Q_OBJECT

    Q_PROPERTY(QVariantList tradeList MEMBER m_lstTrades NOTIFY tradeListChanged)

private:

    std::unique_ptr<Engine::AppEngineLoader> m_loader;
    std::unique_ptr<Engine::AppEngine> m_engine;

    QThread* m_workerThread;

    QString m_loadTrade;
    QVariantList m_lstTrades;
    QVariantList m_lstAPI;
    Engine::API m_curAPI;

    std::atomic<bool> m_hasLoaded{false};
    std::atomic<bool> m_hasStarted{false};

    void setupLoaderConnections();
    void setupEngineConnections();
    void setupConnections();

    explicit AppWorker(QObject* parent = nullptr);
    ~AppWorker();

public:

    static AppWorker *create(QQmlEngine *engine, QJSEngine *scriptEngine) {
        return new AppWorker();
    }

    // ------ Методы доступные в контексте QML ------
    // Запускает движок
    Q_INVOKABLE void run();
    Q_INVOKABLE void loadTradingPairs(const QString& category);
    Q_INVOKABLE void setAPI(const QString& apiKey, const QString& secretKey, bool isTestnet);
    Q_INVOKABLE void checkAPI();
    // Запускает trade по выбранной паре pair
    Q_INVOKABLE void addTrade(const QString& pair);
    // Отбрасывает все пары, которые не заданы в фильтре
    Q_INVOKABLE void filter(const QString& pair, bool on = true);


signals:

    // ------ Сигналы для уведомления об измении свойства ------
    void tradeListChanged();

    void loadingProgress(const QString& step, int current, int total);
    void errorOccurred(const QString& error);
    void loadingFinished(bool success);
    void apiChecked(bool isValid);
    void engineStarted();

    void tickerUpdated(const Engine::stTicker& newTicker);
    void orderBookUpdated(const Engine::stOrderBook& newOrderBook);

};