#include "MarketDataMediator.hpp"
#include "Markets/Bybit/BybitMarketDataService.hpp"
#include "Markets/Bybit/BybitPublicDataStreamer.hpp"
#include "Markets/Bybit/BybitPrivateDataStreamer.hpp"
#include "Markets/Repository/MarketDataRepository.hpp"

namespace Core {

    MarketDataMediator::MarketDataMediator(QObject* parent) : QObject(parent)
    {
        m_MDService = std::make_unique<Core::Markets::BybitMarketDataService>();
        m_publicStreamer = std::make_unique<Core::Markets::BybitPublicDataStreamer>(Tools::MarketType::Spot);
        m_privateStreamer = std::make_unique<Core::Markets::BybitPrivateDataStreamer>();
        m_repository = std::make_unique<Core::Markets::MarketDataRepository>();

        // Связываем Сеть (REST)
        connect(m_MDService.get(), &Markets::IMarketDataService::errorOccurred,
                this, &MarketDataMediator::errorOccurred, Qt::UniqueConnection);

        connect(m_MDService.get(), &Markets::IMarketDataService::downloadProgress,
                this, &MarketDataMediator::downloadProgress, Qt::UniqueConnection);

        connect(m_MDService.get(), &Markets::IMarketDataService::accountVerified,
                this, &MarketDataMediator::onAccountVerificationReady, Qt::UniqueConnection);

        connect(m_MDService.get(), &Markets::IMarketDataService::accountBalanceReceived,
                this, &MarketDataMediator::onAccountBalanceReady, Qt::UniqueConnection);

        connect(m_MDService.get(), &Markets::IMarketDataService::tradePairsReceived,
                this, &MarketDataMediator::onTradePairsReady, Qt::UniqueConnection);

        connect(m_MDService.get(), &Markets::IMarketDataService::klinesReceived,
                this, &MarketDataMediator::onKlinesReady, Qt::UniqueConnection);

        connect(m_MDService.get(), &Markets::IMarketDataService::infoAboutApiReceived,
                this, &MarketDataMediator::onInfoAboutApiReady, Qt::UniqueConnection);

        // Связываем публичный стрим
        connect(m_publicStreamer.get(), &Markets::IPublicMarketDataStreamer::errorOccurred,
                this, &MarketDataMediator::errorOccurredWithId, Qt::UniqueConnection);

        connect(m_publicStreamer.get(), &Markets::IPublicMarketDataStreamer::started,
                this, &MarketDataMediator::streamerStarted, Qt::UniqueConnection);

        connect(m_publicStreamer.get(), &Markets::IPublicMarketDataStreamer::stopped,
                this, &MarketDataMediator::streamerStopped, Qt::UniqueConnection);

        connect(m_publicStreamer.get(), &Markets::IPublicMarketDataStreamer::pingMeasured,
                this, &MarketDataMediator::pingMeasured, Qt::UniqueConnection);

        connect(m_publicStreamer.get(), &Markets::IPublicMarketDataStreamer::klineUpdated,
                this, &MarketDataMediator::onKlineReceived, Qt::UniqueConnection);

        connect(m_publicStreamer.get(), &Markets::IPublicMarketDataStreamer::tickerUpdated,
                this, &MarketDataMediator::tickerReady, Qt::UniqueConnection);

        connect(m_publicStreamer.get(), &Markets::IPublicMarketDataStreamer::orderbookUpdated,
                this, &MarketDataMediator::orderbookReady, Qt::UniqueConnection);

        connect(m_publicStreamer.get(), &Markets::IPublicMarketDataStreamer::publicTradeUpdated,
                this, &MarketDataMediator::onTradesReady, Qt::UniqueConnection);

        // Связываем приватный стрим
        connect(m_privateStreamer.get(), &Markets::IPrivateMarketDataStreamer::errorOccurred,
                this, &MarketDataMediator::errorOccurredWithId, Qt::UniqueConnection);

        connect(m_privateStreamer.get(), &Markets::IPrivateMarketDataStreamer::started,
                this, &MarketDataMediator::streamerStarted, Qt::UniqueConnection);

        connect(m_privateStreamer.get(), &Markets::IPrivateMarketDataStreamer::stopped,
                this, &MarketDataMediator::streamerStopped, Qt::UniqueConnection);

        connect(m_privateStreamer.get(), &Markets::IPrivateMarketDataStreamer::orderUpdated,
                this, &MarketDataMediator::orderReady, Qt::UniqueConnection);

        connect(m_privateStreamer.get(), &Markets::IPrivateMarketDataStreamer::executionUpdated,
                this, &MarketDataMediator::executionReady, Qt::UniqueConnection);

        connect(m_privateStreamer.get(), &Markets::IPrivateMarketDataStreamer::positionUpdated,
                this, &MarketDataMediator::positionReady, Qt::UniqueConnection);

        connect(m_privateStreamer.get(), &Markets::IPrivateMarketDataStreamer::walletUpdated,
                this, &MarketDataMediator::walletReady, Qt::UniqueConnection);

    }

    void MarketDataMediator::runStreamer()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        m_publicStreamer->start();
        m_privateStreamer->start();
    }

    void MarketDataMediator::stopStreamer()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        m_publicStreamer->stop();
        m_privateStreamer->stop();
    }

    bool MarketDataMediator::isPrivateStreamerRunning()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        return m_privateStreamer->isRunning();
    }

    bool MarketDataMediator::isPublicStreamerRunning()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        return m_publicStreamer->isRunning();
    }

    void MarketDataMediator::restartStreamer()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        m_publicStreamer->restart();
        m_privateStreamer->restart();
    }

    void MarketDataMediator::subscribePublicChannel(const QString &symbol)
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        m_publicStreamer->subscribeSymbol(symbol, {
            Markets::PublicStreams::Ticker,
            Markets::PublicStreams::Orderbook,
            Markets::PublicStreams::Kline,
            Markets::PublicStreams::PublicTrade
        });
    }

    void MarketDataMediator::subscribePrivateChannel()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        m_privateStreamer->subscribe({
            Markets::PrivateStreams::Wallet,
            Markets::PrivateStreams::Order,
            Markets::PrivateStreams::Execution,
            Markets::PrivateStreams::Position
        });
    }

    bool MarketDataMediator::loadAllTradePairFromRepository(Tools::MarketType type)
    {
        emit messageSent("Loading trade pairs from repository...");
        auto pairs = m_repository->loadAllFromCryptoRepository(type);

        if (!pairs.isEmpty())
        {
            emit messageSent("Trade pairs ready");
            emit tradePairsReady(pairs);
            return true;
        }

        return false;
    }

    bool MarketDataMediator::loadKlinesFromRepository(Tools::MarketType type, const QString &symbol, const QString &interval, qint64 start, qint64 end)
    {
        return false;
    }

    void MarketDataMediator::loadPublicTradesFromRepository(Tools::MarketType type, const QString &symbol)
    {
        // TODO
    }

    void MarketDataMediator::loadTradePairsFromNetwork(Tools::MarketType type)
    {
        emit messageSent("Loading trade pairs from network...");
        m_MDService->requestTradePairs(type);
    }

    void MarketDataMediator::loadKlinesFromNetwork(Tools::MarketType type, const QString &symbol, const QString &interval, qint64 start, qint64 end)
    {
        m_MDService->requestKlines(type, symbol, interval, start, end);
    }

    void MarketDataMediator::loadAccountBalance()
    {
        emit messageSent("Loading account balance...");
        m_MDService->requestAccountBalance();
    }

    void MarketDataMediator::loadInfoAboutApi()
    {
        emit messageSent("Loading information about your API...");
        m_MDService->requestInfoAboutApi();
    }

    void MarketDataMediator::setApi(const Tools::Api &api)
    {
        m_MDService->setApi(api);
        m_publicStreamer->setApi(api);
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