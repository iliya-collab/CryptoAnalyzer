#include "BybitApiService.hpp"
#include "Tools/Network/Bybit/BybitRestAPI.hpp"
#include "Handlers/RestAPI/BybitTradePairsHandler.hpp"
#include "Handlers/RestAPI/BybitKlineHandler.hpp"
#include "Handlers/RestAPI/BybitAccountBalanceHandler.hpp"
#include "Handlers/RestAPI/BybitInfoAboutAPIHandler.hpp"

namespace Core::Markets
{

    BybitApiService::BybitApiService(QObject* parent)
        : BaseMarketApiService(std::make_unique<Tools::BybitRestAPI>(), parent)
    {
        registerHandler<BybitTradePairsHandler>();
        registerHandler<BybitKlineHandler>();
        registerHandler<BybitAccountBalanceHandler>();
        registerHandler<BybitInfoAboutAPIHandler>();
    }

    void BybitApiService::setApi(const Tools::Api& api)
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (!m_currentApi)
            return;

        m_currentApi->initApi(api);
    }

    void BybitApiService::requestInfoAboutApi()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        requestImpl<BybitInfoAboutAPIHandler>();
    }

    void BybitApiService::requestTradePairs()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        requestImpl<BybitTradePairsHandler>("spot");
    }

    void BybitApiService::requestKlines(const QString& symbol, const QString& interval, qint64 start, qint64 end)
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        requestImpl<BybitKlineHandler>("spot", symbol, interval, start, end);
    }

    void BybitApiService::requestAccountBalance()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        requestImpl<BybitAccountBalanceHandler>();
    }

}

