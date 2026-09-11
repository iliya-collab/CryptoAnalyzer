#include "PostBybitCancelOrderHandler.hpp"

QByteArray Core::Markets::PostBybitCancelOrderHandler::buildRequestBody(const Tools::OrderCancelRequest &req)
{
    QJsonObject body;

    body["category"] = Tools::marketTypeToString(req.m_category);
    body["symbol"] = req.m_symbol;

    if (req.m_orderLinkId.has_value() && !req.m_orderLinkId->isEmpty())
        body["orderLinkId"] = req.m_orderLinkId.value();
    if (req.m_orderId.has_value() && !req.m_orderId->isEmpty())
        body["orderId"] = req.m_orderId.value();

    return QJsonDocument(body).toJson(QJsonDocument::Compact);
}

void Core::Markets::PostBybitCancelOrderHandler::handle(const QJsonObject &data, IPrivateService *service)
{
    if (!data.contains("retMsg") || data["retMsg"].toString() != "OK")
    {
        emit service->errorOccurred(QString("Error processing request [endpoint = %1]: " + data["retMsg"].toString()).arg(endpoint()));
        return;
    }

    QJsonObject result = data["result"].toObject();
    QString orderId = result["orderId"].toString();
    QString orderLinkId = result["orderLinkId"].toString();

    emit service->orderCancelled(orderId, orderLinkId);
}
