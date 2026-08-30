#include "MarketDataService.hpp"

namespace Core {

    MarketDataService::MarketDataService(std::shared_ptr<MarketDataState> state, std::shared_ptr<MarketDataMediator> mediator, QObject *parent) :
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

        connect(mediator.get(), &MarketDataMediator::apiReady,
                state.get(), &MarketDataState::updateApi, Qt::UniqueConnection);

        connect(mediator.get(), &MarketDataMediator::pingMeasured,
                state.get(), &MarketDataState::updatePingMs, Qt::UniqueConnection);

        connect(mediator.get(), &MarketDataMediator::accountVerificationReady,
                state.get(), &MarketDataState::validAccountChanged, Qt::UniqueConnection);

        connect(mediator.get(), &MarketDataMediator::accountBalanceReady,
                state.get(), &MarketDataState::updateBalance, Qt::UniqueConnection);

        connect(mediator.get(), &MarketDataMediator::apiInfoReady,
                state.get(), &MarketDataState::updateApiInfo, Qt::UniqueConnection);

        // --------------------------------------------------------------------------------------

        connect(mediator.get(), &MarketDataMediator::messageSent,
                this, &MarketDataService::messageReceived, Qt::UniqueConnection);

        connect(mediator.get(), &MarketDataMediator::streamerStarted,
                this, &MarketDataService::started, Qt::UniqueConnection);

        connect(mediator.get(), &MarketDataMediator::streamerStopped,
                this, &MarketDataService::stopped, Qt::UniqueConnection);

        connect(mediator.get(), &MarketDataMediator::downloadProgress,
                this, &MarketDataService::downloadProgress, Qt::UniqueConnection);

        connect(mediator.get(), &MarketDataMediator::errorOccurred,
                this, &MarketDataService::onErrorOccurred, Qt::UniqueConnection);
    }

    void MarketDataService::run() {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        m_mediator->runStreamer();
    }

    void MarketDataService::restart() {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        m_mediator->restartStreamer();
    }

    void MarketDataService::shutdown() {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        if (m_mediator->isStreamerRunning())
        {
            m_mediator->stopStreamer();

            QEventLoop loop;
            QTimer::singleShot(5000, &loop, &QEventLoop::quit);

            connect(m_mediator.get(), &Core::MarketDataMediator::streamerStopped, &loop, &QEventLoop::quit, Qt::SingleShotConnection);

            loop.exec();
        }
    }

    void MarketDataService::subscribeSymbol(const QString &symbol)
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        m_mediator->subscribeSymbol(symbol);
    }

    void MarketDataService::loadTradePairs()
    {
        if (!m_mediator->loadAllTradePairFromRepository())
            m_mediator->loadTradePairsFromNetwork();
    }

    void MarketDataService::loadKlines(const QString &symbol, const QString &interval, qint64 start, qint64 end)
    {
        if (!m_mediator->loadKlinesFromRepository(symbol, interval, start, end))
            m_mediator->loadKlinesFromNetwork(symbol, interval, start, end);
    }

    void MarketDataService::loadAccountBalance()
    {
        m_mediator->loadAccountBalance();
    }

    void MarketDataService::loadInfoAboutApi()
    {
        m_mediator->loadInfoAboutApi();
    }

    void MarketDataService::setApi(const Tools::Api &api)
    {
        m_mediator->setApi(api);
        m_mediator->loadAccountBalance();
    }

    void MarketDataService::onErrorOccurred(const QString &error)
    {
        qCritical() << error;
        emit errorOccurred(error);
    }

}
