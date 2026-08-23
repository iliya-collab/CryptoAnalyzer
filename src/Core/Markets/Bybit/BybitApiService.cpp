#include "BybitApiService.hpp"
#include "Tools/Network/Bybit/BybitRestAPI.hpp"
#include "BybitTradePairsHandler.hpp"
#include "BybitKlineHandler.hpp"
#include "BybitAccountBalanceHandler.hpp"

namespace Core::Markets
{

    BybitApiService::BybitApiService(QObject* parent)
        : BaseMarketApiService(std::make_unique<Tools::BybitRestAPI>(), parent)
    {
        registerHandler<BybitTradePairsHandler>();
        registerHandler<BybitKlineHandler>();
        registerHandler<BybitAccountBalanceHandler>();
    }

    void BybitApiService::setAPI(const Tools::API& api)
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (!m_currentApi)
            return;

        m_currentApi->initAPI(api);
    }

    void BybitApiService::requestTradePairs()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        requestTradePairsImpl<BybitTradePairsHandler>("spot");

        // if (!m_currentApi)
        //     return;
        // QUrlQuery params = BybitTradePairsHandler::buildRequest("spot");
        // //params.addQueryItem("category", "spot");
        // m_currentApi->requestEndpoint(BybitTradePairsHandler::endpoint(), params, LOADING_TIMEOUT);
    }

    void BybitApiService::requestKlines(const QString& symbol, const QString& interval, qint64 start, qint64 end)
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        requestKlinesImpl<BybitKlineHandler>("spot", symbol, interval, start, end);

        // if (!m_currentApi)
        //     return;
        // QUrlQuery params = BybitKlineHandler::buildRequest("spot", symbol, interval, start, end);
        // // params.addQueryItem("category", "spot");
        // // params.addQueryItem("symbol", symbol);
        // // params.addQueryItem("interval", interval);
        // // params.addQueryItem("start", QString::number(start));
        // // params.addQueryItem("end", QString::number(end));
        // // params.addQueryItem("limit", "1000");
        // m_currentApi->requestEndpoint(BybitKlineHandler::endpoint(), params, LOADING_TIMEOUT);
    }

    void BybitApiService::requestAccountBalance()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        requestAccountBalanceImpl<BybitAccountBalanceHandler>();

        // if (!m_currentApi)
        //     return;
        // QUrlQuery params = BybitAccountBalanceHandler::buildRequest();
        // m_currentApi->requestEndpoint(BybitAccountBalanceHandler::endpoint(), params, LOADING_TIMEOUT);
    }

}

