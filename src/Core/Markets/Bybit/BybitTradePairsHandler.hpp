#pragma once
#include "Markets/IResponseHandler.hpp"

namespace Core::Markets
{

    class BybitTradePairsHandler : public IResponseHandler
    {
    private:

        void processRequestTradePairs(QList<Tools::TradeInfo>& pairs, const QJsonObject& data);

    public:

        static QString endpoint() { return "/v5/market/instruments-info"; }

        static QUrlQuery buildRequest(const QString& category = "spot")
        {
            QUrlQuery params;
            params.addQueryItem("category", category);
            return params;
        }

        void handle(const QJsonObject& data, IMarketService* service) override;
    };

}

