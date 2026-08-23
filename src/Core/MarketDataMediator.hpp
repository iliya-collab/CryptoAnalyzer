#pragma once
#include "Markets/IMarketService.hpp"
#include "Markets/IMarketDataStreamer.hpp"
#include "Markets/MarketDataRepository.hpp"
#include <QObject>

namespace Core {

class MarketDataMediator : public QObject {
    Q_OBJECT
private:

    std::unique_ptr<Markets::IMarketService> m_apiService;
    std::unique_ptr<Markets::IMarketDataStreamer> m_streamer;
    std::unique_ptr<Markets::MarketDataRepository> m_repository;

public:

    explicit MarketDataMediator(std::unique_ptr<Markets::IMarketService> apiService,
                                std::unique_ptr<Markets::IMarketDataStreamer> streamer,
                                std::unique_ptr<Markets::MarketDataRepository> repository,
                                QObject* parent = nullptr);

    void runStreamer();
    void stopStreamer();
    bool isStreamerRunning();
    void restartStreamer();
    void subscribeSymbol(const QString& symbol);

    bool loadAllTradePairFromRepository();
    bool loadKlinesFromRepository(const QString& symbol, const QString& interval, qint64 start, qint64 end);
    void loadPublicTradesFromRepository(const QString& symbol);

    void loadTradePairsFromNetwork();
    void loadKlinesFromNetwork(const QString& symbol, const QString& interval, qint64 start, qint64 end);
    void loadInfoAboutAccount();

    void setAPI(const Tools::API& api);

signals:

    // Уведомляет об ошибке
    void errorOccurred(const QString& error);
    // Уведомляет об прогрессе загрузки запроса
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    // Уведомляет о старте потоковых данных
    void streamerStarted();
    // Уведомляет о остановке потоковых данных
    void streamerStopped();

    void messageSent(const QString& msg);

    // Уведомляет о валидности ауккаунта
    void accountVerificationReady(bool isValid);
    // Уведомляет о текущем пинге потоковых данных
    void pingMeasured(qint64 pingMs);
    // Уведомляет о получении данных
    void tradePairsReady(const QList<Core::Tools::TradeInfo>& pairs);
    void tickerReady(const Core::Tools::Ticker& ticker);
    void orderBookReady(const Core::Tools::Orderbook& orderbook);
    void klineUpdated(const Core::Tools::Kline& kline);
    void historicalKlinesReady(const QList<Core::Tools::Kline>& klines);
    void tradesReady(const Core::Tools::PublicTrades& trades);
    void apiReady(const Core::Tools::API& api);

private slots:

    void onTradePairsReady(const QList<Core::Tools::TradeInfo>& pairs);
    void onKlineReceived(const Core::Tools::Kline& kline);
    void onKlinesReady(const QList<Core::Tools::Kline>& klines);
    void onTradesReady(const Core::Tools::PublicTrades& trades);
    void onAccountReady(bool isValid);

};

}