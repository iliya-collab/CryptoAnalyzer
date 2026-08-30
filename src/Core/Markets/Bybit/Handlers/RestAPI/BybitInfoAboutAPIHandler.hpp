#pragma once
#include "Markets/IResponseHandler.hpp"

namespace Core::Markets
{

    class BybitInfoAboutAPIHandler : public IResponseHandler
    {
    private:

        QString parseISOFormat(const QString& time);

    public:

        static QString endpoint() { return "/v5/user/query-api"; }

        static QUrlQuery buildRequest() { return QUrlQuery(); }

        void handle(const QJsonObject& data, IMarketService* service) override;
    };

}


