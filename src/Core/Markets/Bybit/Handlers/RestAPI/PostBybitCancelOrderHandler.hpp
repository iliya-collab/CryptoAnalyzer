#pragma once
#include "Markets/RestAPI/IResponseHandler.hpp"
#include "Tools/StdTypes.hpp"

namespace Core::Markets
{

// POST запрос на отмену ордера
class PostBybitCancelOrderHandler : public IPrivateResponseHandler
{
public:

    static QString endpoint() { return "/v5/order/cancel"; }

    static QByteArray buildRequestBody(const Tools::OrderCancelRequest& req);

    void handle(const QJsonObject& data, IPrivateService* service) override;

};

}
