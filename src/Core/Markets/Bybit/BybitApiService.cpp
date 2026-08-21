#include "BybitApiService.hpp"
#include "Tools/Network/Bybit/BybitRestAPI.hpp"
#include "BybitTradePairsStrategy.hpp"
#include "BybitKlineStrategy.hpp"
#include "BybitAccountStrategy.hpp"

namespace Core::Markets
{

    BybitApiService::BybitApiService(QObject* parent)
        : BaseMarketApiService(std::make_unique<Tools::BybitRestAPI>(), parent)
    {
        registerStrategy(std::make_unique<BybitTradePairsStrategy>());
        registerStrategy(std::make_unique<BybitKlineStrategy>());
        registerStrategy(std::make_unique<BybitAccountStrategy>());
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

        if (!m_currentApi)
            return;

        QUrlQuery params;
        params.addQueryItem("category", "spot");

        m_currentApi->requestEndpoint("/v5/market/instruments-info", params, LOADING_TIMEOUT);
    }

    void BybitApiService::requestKlines(const QString& symbol, const QString& interval, qint64 start, qint64 end)
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (!m_currentApi)
            return;

        QUrlQuery params;
        params.addQueryItem("category", "spot");
        params.addQueryItem("symbol", symbol);
        params.addQueryItem("interval", interval);
        params.addQueryItem("start", QString::number(start));
        params.addQueryItem("end", QString::number(end));
        params.addQueryItem("limit", "1000");

        m_currentApi->requestEndpoint("/v5/market/kline", params, LOADING_TIMEOUT);
    }

    void BybitApiService::requestInfoAboutAccount()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (!m_currentApi)
            return;

        m_currentApi->requestEndpoint("/v5/account/info", QUrlQuery(), LOADING_TIMEOUT);
    }

}

