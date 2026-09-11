#pragma once
#include "Markets/RestAPI/IResponseHandler.hpp"

namespace Core::Markets
{

    class GetBybitKlineHandler : public IMarketDataResponseHandler
    {
    private:

        void processRequestKlines(QList<Tools::Kline>& klines, const QString& interval, const QJsonObject& data);

    public:

        static QString endpoint() { return "/v5/market/kline"; }

        static QUrlQuery buildRequest(Tools::MarketType type, const QString& symbol, const QString& interval, qint64 start, qint64 end, int limit = 1000)
        {
            QUrlQuery params;
            params.addQueryItem("category", Tools::marketTypeToString(type));
            params.addQueryItem("symbol", symbol);
            params.addQueryItem("interval", interval);
            params.addQueryItem("start", QString::number(start));
            params.addQueryItem("end", QString::number(end));
            params.addQueryItem("limit", QString::number(limit));
            return params;
        }

        void handle(const QJsonObject& data, IMarketDataService* service) override;

    };
}