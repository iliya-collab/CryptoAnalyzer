#pragma once
#include "Markets/RestAPI/IResponseHandler.hpp"
#include "Tools/StdTypes.hpp"

namespace Core::Markets
{

// POST запрос на создания ордера
class PostBybitCreateOrderHandler : public IPrivateResponseHandler
{
public:

    static QString endpoint() { return "/v5/order/create"; }

    static QByteArray buildRequestBody(const Tools::OrderRequest& req);

    void handle(const QJsonObject& data, IPrivateService* service) override;

};

}