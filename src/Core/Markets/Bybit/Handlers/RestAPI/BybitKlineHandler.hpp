#pragma once
#include "Markets/IResponseHandler.hpp"

namespace Core::Markets
{

    class BybitKlineHandler : public IResponseHandler
    {
    private:

        void processRequestKlines(QList<Tools::Kline>& klines, const QString& interval, const QJsonObject& data);

    public:

        static QString endpoint() { return "/v5/market/kline"; }

        static QUrlQuery buildRequest(const QString& category, const QString& symbol, const QString& interval, qint64 start, qint64 end, int limit = 1000)
        {
            QUrlQuery params;
            params.addQueryItem("category", category);
            params.addQueryItem("symbol", symbol);
            params.addQueryItem("interval", interval);
            params.addQueryItem("start", QString::number(start));
            params.addQueryItem("end", QString::number(end));
            params.addQueryItem("limit", QString::number(limit));
            return params;
        }

        void handle(const QJsonObject& data, IMarketService* service) override;

    };
}