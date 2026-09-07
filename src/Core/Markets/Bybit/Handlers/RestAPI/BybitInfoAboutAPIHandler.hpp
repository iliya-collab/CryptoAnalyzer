#pragma once
#include "Markets/RestAPI/IResponseHandler.hpp"

namespace Core::Markets
{

    class BybitInfoAboutAPIHandler : public IMarketDataResponseHandler
    {
    private:

        QString parseISOFormat(const QString& time);

    public:

        static QString endpoint() { return "/v5/user/query-api"; }

        static QUrlQuery buildRequest() { return QUrlQuery(); }

        void handle(const QJsonObject& data, IMarketDataService* service) override;
    };

}


