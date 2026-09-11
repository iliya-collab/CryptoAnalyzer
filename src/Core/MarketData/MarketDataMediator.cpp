#include "MarketDataMediator.hpp"
#include "Markets/Bybit/BybitMarketDataService.hpp"
#include "Markets/Bybit/BybitMarketDataStreamer.hpp"
#include "Markets/Repository/MarketDataRepository.hpp"

namespace Core {

    MarketDataMediator::MarketDataMediator(QObject* parent) : QObject(parent)
    {
        m_service = std::make_unique<Core::Markets::BybitMarketDataService>();
        m_publicStreamer = std::make_unique<Core::Markets::BybitMarketDataStreamer>(Tools::MarketType::Spot);
        m_repository = std::make_unique<Core::Markets::MarketDataRepository>();

        // Связываем Сеть (REST)
        connect(m_service.get(), &Markets::IMarketDataService::errorOccurred,
                this, &MarketDataMediator::errorOccurred, Qt::UniqueConnection);

        connect(m_service.get(), &Markets::IMarketDataService::downloadProgress,
                this, &MarketDataMediator::downloadProgress, Qt::UniqueConnection);

        connect(m_service.get(), &Markets::IMarketDataService::tradePairsReceived,
                this, &MarketDataMediator::onTradePairsReady, Qt::UniqueConnection);

        connect(m_service.get(), &Markets::IMarketDataService::klinesReceived,
                this, &MarketDataMediator::onKlinesReady, Qt::UniqueConnection);

        // Связываем публичный стрим
        connect(m_publicStreamer.get(), &Markets::IMarketDataStreamer::errorOccurred,
                this, &MarketDataMediator::errorOccurredWithId, Qt::UniqueConnection);

        connect(m_publicStreamer.get(), &Markets::IMarketDataStreamer::started,
                this, &MarketDataMediator::streamerStarted, Qt::UniqueConnection);

        connect(m_publicStreamer.get(), &Markets::IMarketDataStreamer::stopped,
                this, &MarketDataMediator::streamerStopped, Qt::UniqueConnection);

        connect(m_publicStreamer.get(), &Markets::IMarketDataStreamer::pingMeasured,
                this, &MarketDataMediator::pingMeasured, Qt::UniqueConnection);

        connect(m_publicStreamer.get(), &Markets::IMarketDataStreamer::klineUpdated,
                this, &MarketDataMediator::onKlineReceived, Qt::UniqueConnection);

        connect(m_publicStreamer.get(), &Markets::IMarketDataStreamer::tickerUpdated,
                this, &MarketDataMediator::tickerReady, Qt::UniqueConnection);

        connect(m_publicStreamer.get(), &Markets::IMarketDataStreamer::orderbookUpdated,
                this, &MarketDataMediator::orderbookReady, Qt::UniqueConnection);

        connect(m_publicStreamer.get(), &Markets::IMarketDataStreamer::publicTradeUpdated,
                this, &MarketDataMediator::onTradesReady, Qt::UniqueConnection);

    }

    void MarketDataMediator::runStreamer()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        m_publicStreamer->start();
    }

    void MarketDataMediator::stopStreamer()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        m_publicStreamer->stop();
    }

    bool MarketDataMediator::isStreamerRunning()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        return m_publicStreamer->isRunning();
    }

    void MarketDataMediator::restartStreamer()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        m_publicStreamer->restart();
    }

    void MarketDataMediator::subscribe(const QString &symbol)
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        m_publicStreamer->subscribeSymbol(symbol, {
            Markets::PublicStreams::Ticker,
            Markets::PublicStreams::Orderbook,
            Markets::PublicStreams::Kline,
            Markets::PublicStreams::PublicTrade
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
        m_service->requestTradePairs(type);
    }

    void MarketDataMediator::loadKlinesFromNetwork(Tools::MarketType type, const QString &symbol, const QString &interval, qint64 start, qint64 end)
    {
        m_service->requestKlines(type, symbol, interval, start, end);
    }

    void MarketDataMediator::init(bool isTestnet)
    {
        m_service->init(isTestnet);
        m_publicStreamer->init(isTestnet);
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

}