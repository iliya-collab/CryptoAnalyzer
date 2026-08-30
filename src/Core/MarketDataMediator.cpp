#include "MarketDataMediator.hpp"

namespace Core {

    MarketDataMediator::MarketDataMediator(std::unique_ptr<Markets::IMarketService> apiService,
                                        std::unique_ptr<Markets::IMarketDataStreamer> streamer,
                                        std::unique_ptr<Markets::MarketDataRepository> repository,
                                        QObject* parent) :
        m_apiService(std::move(apiService)),
        m_streamer(std::move(streamer)),
        m_repository(std::move(repository)),
        QObject(parent)
    {
        // Связываем Сеть (REST)
        connect(m_apiService.get(), &Markets::IMarketService::errorOccurred,
                this, &MarketDataMediator::errorOccurred, Qt::UniqueConnection);

        connect(m_apiService.get(), &Markets::IMarketService::downloadProgress,
                this, &MarketDataMediator::downloadProgress, Qt::UniqueConnection);

        connect(m_apiService.get(), &Markets::IMarketService::accountVerified,
                this, &MarketDataMediator::onAccountVerificationReady, Qt::UniqueConnection);

        connect(m_apiService.get(), &Markets::IMarketService::accountBalanceReceived,
                this, &MarketDataMediator::onAccountBalanceReady, Qt::UniqueConnection);

        connect(m_apiService.get(), &Markets::IMarketService::tradePairsReceived,
                this, &MarketDataMediator::onTradePairsReady, Qt::UniqueConnection);

        connect(m_apiService.get(), &Markets::IMarketService::klinesReceived,
                this, &MarketDataMediator::onKlinesReady, Qt::UniqueConnection);

        connect(m_apiService.get(), &Markets::IMarketService::infoAboutApiReceived,
                this, &MarketDataMediator::onInfoAboutApiReady, Qt::UniqueConnection);

        // Связываем Стрим (WebSocket)
        connect(m_streamer.get(), &Markets::IMarketDataStreamer::errorOccurred,
                this, &MarketDataMediator::errorOccurred, Qt::UniqueConnection);

        connect(m_streamer.get(), &Markets::IMarketDataStreamer::started,
                this, &MarketDataMediator::streamerStarted, Qt::UniqueConnection);

        connect(m_streamer.get(), &Markets::IMarketDataStreamer::stopped,
                this, &MarketDataMediator::streamerStopped, Qt::UniqueConnection);

        connect(m_streamer.get(), &Markets::IMarketDataStreamer::pingMeasured,
                this, &MarketDataMediator::pingMeasured, Qt::UniqueConnection);

        connect(m_streamer.get(), &Markets::IMarketDataStreamer::klineUpdated,
                this, &MarketDataMediator::onKlineReceived, Qt::UniqueConnection);

        connect(m_streamer.get(), &Markets::IMarketDataStreamer::tickerUpdated,
                this, &MarketDataMediator::tickerReady, Qt::UniqueConnection);

        connect(m_streamer.get(), &Markets::IMarketDataStreamer::orderbookUpdated,
                this, &MarketDataMediator::orderbookReady, Qt::UniqueConnection);

        connect(m_streamer.get(), &Markets::IMarketDataStreamer::publicTradeUpdated,
                this, &MarketDataMediator::onTradesReady, Qt::UniqueConnection);


    }

    void MarketDataMediator::runStreamer()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        m_streamer->start();
    }

    void MarketDataMediator::stopStreamer()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        m_streamer->stop();
    }

    bool MarketDataMediator::isStreamerRunning()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        return m_streamer->hasRunned();
    }

    void MarketDataMediator::restartStreamer()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        m_streamer->restart();
    }

    void MarketDataMediator::subscribeSymbol(const QString &symbol)
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        m_streamer->subscribeSymbol(symbol, {
                                                Markets::WebSocketStreams::Ticker,
                                                Markets::WebSocketStreams::Orderbook,
                                                Markets::WebSocketStreams::Kline,
                                                Markets::WebSocketStreams::PublicTrade
                                            });
    }

    bool MarketDataMediator::loadAllTradePairFromRepository()
    {
        emit messageSent("Loading trade pairs from repository...");
        auto pairs = m_repository->loadAllFromCryptoRepository();

        if (!pairs.isEmpty())
        {
            emit messageSent("Trade pairs ready");
            emit tradePairsReady(pairs);
            return true;
        }

        return false;
    }

    bool MarketDataMediator::loadKlinesFromRepository(const QString &symbol, const QString &interval, qint64 start, qint64 end)
    {
        return false;
    }

    void MarketDataMediator::loadPublicTradesFromRepository(const QString &symbol)
    {
        // TODO
    }

    void MarketDataMediator::loadTradePairsFromNetwork()
    {
        emit messageSent("Loading trade pairs from network...");
        m_apiService->requestTradePairs();
    }

    void MarketDataMediator::loadKlinesFromNetwork(const QString &symbol, const QString &interval, qint64 start, qint64 end)
    {
        m_apiService->requestKlines(symbol, interval, start, end);
    }

    void MarketDataMediator::loadAccountBalance()
    {
        emit messageSent("Loading account balance...");
        m_apiService->requestAccountBalance();
    }

    void MarketDataMediator::loadInfoAboutApi()
    {
        emit messageSent("Loading information about your API...");
        m_apiService->requestInfoAboutApi();
    }

    void MarketDataMediator::setApi(const Tools::Api &api)
    {
        m_apiService->setApi(api);
        m_streamer->setApi(api);
        emit apiReady(api);
    }

    void MarketDataMediator::onTradePairsReady(const QList<Tools::TradeInfo> &pairs)
    {
        emit messageSent("Trade pairs ready");
        m_repository->saveToCryptoRepository(pairs);
        emit tradePairsReady(pairs);
    }

    void MarketDataMediator::onKlineReceived(const Tools::Kline& kline)
    {
        if (kline.m_confirm)
        {
            m_repository->saveToKlineRepository(kline);
        }
        emit klineUpdated(kline);
    }

    void MarketDataMediator::onKlinesReady(const QList<Tools::Kline>& klines)
    {
        emit messageSent("Klines ready");
        m_repository->saveToKlinesRepository(klines);
        emit historicalKlinesReady(klines);
    }

    void MarketDataMediator::onTradesReady(const Tools::PublicTrades& trades)
    {
        emit tradesReady(trades);
    }

    void MarketDataMediator::onAccountVerificationReady()
    {
        emit messageSent("Account verified");
        emit accountVerificationReady();
    }

    void MarketDataMediator::onAccountBalanceReady(const Core::Tools::AccountBalance& balance)
    {
        emit messageSent("Account balance ready");
        emit accountBalanceReady(balance);
    }

    void MarketDataMediator::onInfoAboutApiReady(const Tools::ApiInfo &apiInfo)
    {
        emit messageSent("The information about your API ready");
        emit apiInfoReady(apiInfo);
    }

}