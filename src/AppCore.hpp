#pragma once
#include <QQmlContext>
#include <QObject>
#include <QThread>
#include <atomic>
#include <memory>
#include "Engine/AppEngine.hpp"
#include "Engine/AppEngineLoader.hpp"
#include "Engine/Tools/DataModels/OrderbookModel.hpp"

class AppCore : public QObject {
    Q_OBJECT

    Q_PROPERTY(QVariantList tradeList MEMBER m_tradeList NOTIFY tradeListChanged)
    Q_PROPERTY(QVariantList loadedCandles MEMBER m_loadedCandles NOTIFY loadedCandlesChanged)
    Q_PROPERTY(qint64 startTime MEMBER m_startTime NOTIFY startTimeChanged)

private:

    std::unique_ptr<Engine::AppEngineLoader> m_loader;
    std::unique_ptr<Engine::AppEngine> m_engine;

    QThread* m_workerThread;

    qint64 m_startTime = 0;
    QString m_lastTrade = "";
    QVariantList m_tradeList;
    QVariantList m_loadedCandles;
    Engine::API m_curAPI;
    std::atomic<bool> wasInit{false};

    void setupLoaderConnections();
    void setupEngineConnections();
    void setupConnections();

    explicit AppCore(QObject* parent = nullptr);
    ~AppCore();

public:

    static AppCore *create(QQmlEngine *engine, QJSEngine *scriptEngine);

    // ------ Методы доступные в контексте QML ------
    Q_INVOKABLE void init();
    // Запускает движок
    Q_INVOKABLE void run();
    // Перезапускает движок
    Q_INVOKABLE void restart();
    // Прерывает работу движка
    Q_INVOKABLE void interrupt();

    Q_INVOKABLE void loadTradesFromRepository(const QString& category = "ALL");
    Q_INVOKABLE void loadTradesFromNetwork();
    Q_INVOKABLE void loadCandlesFromNetwork(const QString& symbol, const QString& interval, int start, int end);
    Q_INVOKABLE void saveCandle(const Engine::ItemCandle& candle);

    Q_INVOKABLE void setAPI(const QString& apiKey, const QString& secretKey, bool isTestnet);
    Q_INVOKABLE void checkAPI();
    // Запускает trade по выбранной паре pair
    Q_INVOKABLE void addTrade(const QString& pair);


signals:

    // Сигнал для уведомления об измении свойства tradeList
    void tradeListChanged();
    void loadedCandlesChanged();
    void startTimeChanged(qint64 msTime);

    void errorOccurred(const QString& error);
    void messageReceived(const QString& msg);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void apiChecked(bool isValid);
    void engineStarted();
    void engineStopped();

    void tickerUpdated(const Engine::Ticker& newTicker);
    void orderbookUpdated(const Engine::Orderbook& newOrderbook);
    void klineUpdated(const Engine::Kline& newKline);

};