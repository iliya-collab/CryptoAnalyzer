#pragma once
#include "Markets/RestAPI/IMarketDataService.hpp"
#include "Markets/Websocket/IMarketDataStreamer.hpp"
#include "Markets/Repository/MarketDataRepository.hpp"
#include <QObject>

namespace Core {

class MarketDataMediator : public QObject
{
    Q_OBJECT
private:

    std::unique_ptr<Markets::IMarketDataService> m_service;
    std::unique_ptr<Markets::IMarketDataStreamer> m_publicStreamer;
    std::unique_ptr<Markets::MarketDataRepository> m_repository;

public:

    explicit MarketDataMediator(QObject* parent = nullptr);

    void runStreamer();
    void stopStreamer();
    bool isStreamerRunning();
    void restartStreamer();
    void subscribe(const QString& symbol);

    bool loadAllTradePairFromRepository(Tools::MarketType type);
    bool loadKlinesFromRepository(Tools::MarketType type, const QString& symbol, const QString& interval, qint64 start, qint64 end);
    void loadPublicTradesFromRepository(Tools::MarketType type, const QString& symbol);

    void loadTradePairsFromNetwork(Tools::MarketType type);
    void loadKlinesFromNetwork(Tools::MarketType type, const QString& symbol, const QString& interval, qint64 start, qint64 end);

    void init(bool isTestnet);

signals:

    // Уведомляет об ошибке
    void errorOccurredWithId(const QString& id, const QString& error);
    void errorOccurred(const QString& error);
    // Уведомляет об прогрессе загрузки запроса
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    // Уведомляет о старте потоковых данных
    void streamerStarted(const QString& id);
    // Уведомляет о остановке потоковых данных
    void streamerStopped(const QString& id);

    void messageSent(const QString& msg);

    // Уведомляет о получении данных
    void pingMeasured(qint64 pingMs);
    void tradePairsReady(const QList<Core::Tools::TradeInfo>& pairs);
    void tickerReady(const Core::Tools::Ticker& ticker);
    void orderbookReady(const Core::Tools::Orderbook& orderbook);
    void klineUpdated(const Core::Tools::Kline& kline);
    void historicalKlinesReady(const QList<Core::Tools::Kline>& klines);
    void tradesReady(const Core::Tools::PublicTrades& trades);

private slots:

    void onTradePairsReady(const QList<Core::Tools::TradeInfo>& pairs);
    void onKlineReceived(const Core::Tools::Kline& kline);
    void onKlinesReady(const QList<Core::Tools::Kline>& klines);
    void onTradesReady(const Core::Tools::PublicTrades& trades);

};

}