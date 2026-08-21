#include "BybitApiService.hpp"

namespace Core::Markets {

    BybitApiService::BybitApiService(QObject* parent) : IMarketApiService(parent)
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        m_currentApi = std::make_unique<Tools::BybitRestAPI>(this);

        connect(m_currentApi.get(), &Tools::BybitRestAPI::downloadProgress, this, &BybitApiService::downloadProgress, Qt::UniqueConnection);
        connect(m_currentApi.get(), &Tools::BybitRestAPI::errorOccurred, this, &BybitApiService::errorOccurred, Qt::UniqueConnection);
        connect(m_currentApi.get(), &Tools::BybitRestAPI::dataReceived, this, &BybitApiService::onDataReceived, Qt::UniqueConnection);
    }

    void BybitApiService::setAPI(const Tools::API& api)
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (!m_currentApi)
            return;

        m_currentApi->initAPI(api);
    }

    void BybitApiService::processRequestKlines(QList<Tools::Kline>& klines, const QString& interval, const QJsonObject& data)
    {
        QJsonObject result = data["result"].toObject();
        QString symbol = result["symbol"].toString();
        QJsonArray list = result["list"].toArray();

        QMutexLocker locker(&m_mutex);
        for (const auto& obj : list) {
            QJsonArray itemArr = obj.toArray();
            Tools::Kline kline;
            kline.m_symbol = symbol;
            kline.m_start = itemArr[0].toString().toDouble();
            kline.m_end = kline.m_start + 60000;
            kline.m_open = itemArr[1].toString().toDouble();
            kline.m_high = itemArr[2].toString().toDouble();
            kline.m_low = itemArr[3].toString().toDouble();
            kline.m_close = itemArr[4].toString().toDouble();
            kline.m_volume = itemArr[5].toString().toDouble();
            kline.m_turnover = itemArr[6].toString().toDouble();
            kline.m_confirm = true;
            kline.m_interval = interval;
            klines.append(kline);
        }
    }

    void BybitApiService::onDataReceived(const QUrl &reqUrl, const QJsonObject &data)
    {
        if (m_requestedUrls.contains(reqUrl))
        {
            //qInfo() << reqUrl.toString();
            m_requestedUrls.value(reqUrl)(data);
            m_requestedUrls.remove(reqUrl);
        }
    }

    void BybitApiService::onInfoAboutAccountReceived(const QJsonObject &data)
    {
        emit infoAboutAccountReceived(data["retMsg"] == "OK");
    }

    void BybitApiService::onTradePairsReceived(const QJsonObject &data)
    {
        QList<Tools::TradeInfo> pairs;
        processRequestTradePairs(pairs, data);
        emit tradePairsReceived(pairs);
    }

    void BybitApiService::onKlinesReceived(const QJsonObject &data)
    {
        QList<Tools::Kline> klines;
        processRequestKlines(klines, "1", data);
        emit klinesReceived(klines);
    }

    void BybitApiService::processRequestTradePairs(QList<Tools::TradeInfo>& pairs, const QJsonObject& data)
    {
        QJsonObject result = data["result"].toObject();
        QString category = result["category"].toString();
        QJsonArray list = result["list"].toArray();

        QMutexLocker locker(&m_mutex);
        for (const auto& obj : list) {
            QJsonObject item = obj.toObject();
            Tools::TradeInfo info;
            info.symbol = item["symbol"].toString();
            info.base_coin = item["baseCoin"].toString();
            info.quote_coin = item["quoteCoin"].toString();
            pairs.append(info);
        }
    }

    void BybitApiService::requestTradePairs()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (!m_currentApi)
            return;

        QUrlQuery params;
        params.addQueryItem("category", "spot");

        auto url = m_currentApi->requestEndpoint("/v5/market/instruments-info", params, LOADING_TIMEOUT);
        if (!m_requestedUrls.contains(url))
            m_requestedUrls[url] = [this](const QJsonObject& data) { onTradePairsReceived(data); };
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

        auto url = m_currentApi->requestEndpoint("/v5/market/kline", params, LOADING_TIMEOUT);
        if (!m_requestedUrls.contains(url))
            m_requestedUrls[url] = [this](const QJsonObject& data) { onKlinesReceived(data); };
    }

    void BybitApiService::requestInfoAboutAccount()
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (!m_currentApi)
            return;

        auto url = m_currentApi->requestEndpoint("/v5/account/info", QUrlQuery(), LOADING_TIMEOUT);
        if (!m_requestedUrls.contains(url))
            m_requestedUrls[url] = [this](const QJsonObject& data) { onInfoAboutAccountReceived(data); };
    }

}

