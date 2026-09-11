#pragma once
#include "Markets/RestAPI/IResponseHandler.hpp"

namespace Core::Markets
{

    class GetBybitInfoAboutAPIHandler : public IPrivateResponseHandler
    {
    private:

        QString parseISOFormat(const QString& time);

    public:

        static QString endpoint() { return "/v5/user/query-api"; }

        static QUrlQuery buildRequest() { return QUrlQuery(); }

        void handle(const QJsonObject& data, IPrivateService* service) override;
    };

}


