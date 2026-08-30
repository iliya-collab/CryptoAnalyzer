#pragma once
#include "Markets/IResponseHandler.hpp"

namespace Core::Markets
{

    class BybitAccountBalanceHandler : public IResponseHandler
    {
    public:

        static QString endpoint() { return "/v5/account/wallet-balance"; }

        static QUrlQuery buildRequest()
        {
            QUrlQuery query;
            query.addQueryItem("accountType", "UNIFIED");
            return query;
        }

        void handle(const QJsonObject& data, IMarketService* service) override;
    };

}

