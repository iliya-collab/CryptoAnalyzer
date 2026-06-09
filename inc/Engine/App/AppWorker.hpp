#pragma once

#include <QQmlContext>

#include <QObject>
#include <QThread>
#include <memory>

#include "Engine/App/AppEngine.hpp"
#include "Engine/App/AppEngineLoader.hpp"

#include "Engine/Tools/OrderBookModel.hpp"

class AppWorker : public QObject {
    Q_OBJECT

    Q_PROPERTY(QVariantList tradeList MEMBER m_lstTrades NOTIFY tradeListChanged)

private:

    std::unique_ptr<Engine::AppEngineLoader> m_loader;
    std::unique_ptr<Engine::AppEngine> m_engine;

    QThread* m_workerThread;

    QString m_lastTrade = "";
    QVariantList m_lstTrades;
    QVariantList m_lstAPI;
    Engine::API m_curAPI;

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
    // Перезапускает движок
    Q_INVOKABLE void restart();
    // Прерывает работу движка
    Q_INVOKABLE void interrupt();
    Q_INVOKABLE void loadTradingPairs(const QString& category);
    Q_INVOKABLE void setAPI(const QString& apiKey, const QString& secretKey, bool isTestnet);
    Q_INVOKABLE void checkAPI();
    // Запускает trade по выбранной паре pair
    Q_INVOKABLE void addTrade(const QString& pair);


signals:

    // ------ Сигналы для уведомления об измении свойства ------
    void tradeListChanged();

    void loadingProgress(const QString& step, int current, int total);
    void errorOccurred(const QString& error);
    void loadingFinished(bool success);
    void apiChecked(bool isValid);
    void engineStarted();
    void engineStopped();

    void tickerUpdated(const Engine::stTicker& newTicker);
    void orderBookUpdated(const Engine::stOrderBook& newOrderBook);
    void klineUpdated(const Engine::stKline& newKline);

};