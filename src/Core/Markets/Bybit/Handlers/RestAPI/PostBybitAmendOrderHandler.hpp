#pragma once
#include "Markets/RestAPI/IResponseHandler.hpp"
#include "Tools/StdTypes.hpp"

namespace Core::Markets
{

// POST запрос на исправление ордера
class PostBybitAmendOrderHandler : public IPrivateResponseHandler
{
public:

    static QString endpoint() { return "/v5/order/amend"; }

    static QByteArray buildRequestBody(const Tools::OrderAmendRequest& req);

    void handle(const QJsonObject& data, IPrivateService* service) override;

};

}
