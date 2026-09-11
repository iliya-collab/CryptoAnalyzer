#pragma once
#include "Markets/RestAPI/IResponseHandler.hpp"

namespace Core::Markets
{

    class GetBybitAccountBalanceHandler : public IPrivateResponseHandler
    {
    public:

        static QString endpoint() { return "/v5/account/wallet-balance"; }

        static QUrlQuery buildRequest()
        {
            QUrlQuery query;
            query.addQueryItem("accountType", "UNIFIED");
            return query;
        }

        void handle(const QJsonObject& data, IPrivateService* service) override;
    };

}

