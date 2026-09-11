#pragma once
#include "Markets/RestAPI/IResponseHandler.hpp"

namespace Core::Markets
{

    class GetBybitTradePairsHandler : public IMarketDataResponseHandler
    {
    private:

        void processRequestTradePairs(QList<Tools::TradeInfo>& pairs, const QJsonObject& data);

    public:

        static QString endpoint() { return "/v5/market/instruments-info"; }

        static QUrlQuery buildRequest(Tools::MarketType type)
        {
            QUrlQuery params;
            params.addQueryItem("category", Tools::marketTypeToString(type));
            return params;
        }

        void handle(const QJsonObject& data, IMarketDataService* service) override;
    };

}

