#include "MarketDataService.hpp"

namespace Core
{

    MarketDataService::MarketDataService(std::shared_ptr<MarketDataState> state,
                                     std::shared_ptr<MarketDataMediator> mediator,
                                     QObject *parent) :
        m_state(state), m_mediator(mediator), QObject(parent)
    {
        connect(mediator.get(), &MarketDataMediator::tickerReady,
                state.get(), &MarketDataState::updateTicker, Qt::UniqueConnection);

        connect(mediator.get(), &MarketDataMediator::orderbookReady,
                state.get(), &MarketDataState::updateOrderbook, Qt::UniqueConnection);

        connect(mediator.get(), &MarketDataMediator::klineUpdated,
                state.get(), &MarketDataState::updateKline, Qt::UniqueConnection);

        connect(mediator.get(), &MarketDataMediator::historicalKlinesReady,
                state.get(), &MarketDataState::addHistoricalKlines, Qt::UniqueConnection);

        connect(mediator.get(), &MarketDataMediator::tradePairsReady,
                state.get(), &MarketDataState::updateTradePairs, Qt::UniqueConnection);

        connect(mediator.get(), &MarketDataMediator::tradesReady,
                state.get(), &MarketDataState::updateTrades, Qt::UniqueConnection);

        connect(mediator.get(), &MarketDataMediator::pingMeasured,
                state.get(), &MarketDataState::updatePingMs, Qt::UniqueConnection);

        // --------------------------------------------------------------------------------------

        connect(mediator.get(), &MarketDataMediator::messageSent,
                this, &MarketDataService::messageReceived, Qt::UniqueConnection);

        connect(mediator.get(), &MarketDataMediator::streamerStarted,
                this, &MarketDataService::onStreamerStarted, Qt::UniqueConnection);

        connect(mediator.get(), &MarketDataMediator::streamerStopped,
                this, &MarketDataService::onStreamerStopped, Qt::UniqueConnection);

        connect(mediator.get(), &MarketDataMediator::downloadProgress,
                this, &MarketDataService::downloadProgress, Qt::UniqueConnection);

        connect(mediator.get(), &MarketDataMediator::errorOccurredWithId,
                this, &MarketDataService::onErrorOccurredWithId, Qt::UniqueConnection);

        connect(mediator.get(), &MarketDataMediator::errorOccurred,
                this, &MarketDataService::errorOccurred, Qt::UniqueConnection);
    }

    void MarketDataService::run()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        m_mediator->runStreamer();
    }

    void MarketDataService::restart()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        m_mediator->restartStreamer();
    }

    void MarketDataService::shutdown()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        m_mediator->stopStreamer();

        QEventLoop loop;
        QTimer::singleShot(5000, &loop, &QEventLoop::quit);

        connect(m_mediator.get(), &Core::MarketDataMediator::streamerStopped, &loop, &QEventLoop::quit, Qt::SingleShotConnection);

        loop.exec();

    }

    void MarketDataService::init(bool isTestnet)
    {
        m_mediator->init(isTestnet);
    }

    void MarketDataService::subscribeSymbol(const QString &symbol)
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        m_mediator->subscribe(symbol);
        // m_mediator->subscribePrivateChannel();
    }

    void MarketDataService::loadTradePairs(Core::Tools::MarketType type)
    {
        if (!m_mediator->loadAllTradePairFromRepository(type))
            m_mediator->loadTradePairsFromNetwork(type);
    }

    void MarketDataService::loadKlines(Core::Tools::MarketType type, const QString &symbol, const QString &interval, qint64 start, qint64 end)
    {
        if (!m_mediator->loadKlinesFromRepository(type, symbol, interval, start, end))
            m_mediator->loadKlinesFromNetwork(type, symbol, interval, start, end);
    }

    // void MarketDataService::loadAccountBalance()
    // {
    //     m_mediator->loadAccountBalance();
    // }

    // void MarketDataService::loadInfoAboutApi()
    // {
    //     m_mediator->loadInfoAboutApi();
    // }

    // void MarketDataService::setApi(const Tools::Api &api)
    // {
    //     m_mediator->setApi(api);
    //     m_mediator->loadAccountBalance();
    // }

    void MarketDataService::onErrorOccurredWithId(const QString& id, const QString &error)
    {
        QString errMsg = id + " : " + error;
        qCritical().noquote() << errMsg;
        emit errorOccurred(errMsg);
    }

    void MarketDataService::onStreamerStarted(const QString& id)
    {
        qDebug().noquote() << QString("%1 : started").arg(id);
        emit streamerStarted(id);
    }

    void MarketDataService::onStreamerStopped(const QString& id)
    {
        qDebug().noquote() << QString("%1 : stopped").arg(id);
        emit streamerStopped(id);
    }

}
