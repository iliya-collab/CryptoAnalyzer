#include "BybitMarketDataService.hpp"
#include "Tools/Network/Bybit/BybitRestAPI.hpp"
#include "Handlers/RestAPI/BybitTradePairsHandler.hpp"
#include "Handlers/RestAPI/BybitKlineHandler.hpp"
#include "Handlers/RestAPI/BybitAccountBalanceHandler.hpp"
#include "Handlers/RestAPI/BybitInfoAboutAPIHandler.hpp"

namespace Core::Markets
{

    BybitMarketDataService::BybitMarketDataService(QObject* parent)
        : BaseMarketDataService(std::make_unique<Tools::BybitRestAPI>(parent), parent)
    {
        registerHandler<BybitTradePairsHandler>();
        registerHandler<BybitKlineHandler>();
        registerHandler<BybitAccountBalanceHandler>();
        registerHandler<BybitInfoAboutAPIHandler>();
    }

    void BybitMarketDataService::setApi(const Tools::Api& api)
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (!m_currentApi)
            return;

        m_currentApi->init(api);
    }

    void BybitMarketDataService::requestInfoAboutApi()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        requestImpl<BybitInfoAboutAPIHandler>();
    }

    void BybitMarketDataService::requestTradePairs(Tools::MarketType type)
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        requestImpl<BybitTradePairsHandler>(type);
    }

    void BybitMarketDataService::requestKlines(Tools::MarketType type, const QString& symbol, const QString& interval, qint64 start, qint64 end)
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        requestImpl<BybitKlineHandler>(type, symbol, interval, start, end);
    }

    void BybitMarketDataService::requestAccountBalance()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        requestImpl<BybitAccountBalanceHandler>();
    }

}

