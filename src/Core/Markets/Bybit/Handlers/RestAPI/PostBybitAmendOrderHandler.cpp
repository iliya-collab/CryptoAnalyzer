#include "PostBybitAmendOrderHandler.hpp"

QByteArray Core::Markets::PostBybitAmendOrderHandler::buildRequestBody(const Tools::OrderAmendRequest &req)
{
    QJsonObject body;

    body["category"] = Tools::marketTypeToString(req.m_category);
    body["symbol"] = req.m_symbol;

    if (req.m_orderLinkId.has_value() && !req.m_orderLinkId->isEmpty())
        body["orderLinkId"] = req.m_orderLinkId.value();
    if (req.m_orderId.has_value() && !req.m_orderId->isEmpty())
        body["orderId"] = req.m_orderId.value();
    if (req.m_qty.has_value() && !req.m_qty->isEmpty())
        body["qty"] = req.m_qty.value();
    if (req.m_price.has_value() && !req.m_price->isEmpty())
        body["price"] = req.m_price.value();
    if (req.m_takeProfit.has_value() && !req.m_takeProfit->isEmpty())
        body["takeProfit"] = req.m_takeProfit.value();
    if (req.m_stopLoss.has_value() && !req.m_stopLoss->isEmpty())
        body["stopLoss"] = req.m_stopLoss.value();
    if (req.m_triggerPrice.has_value() && !req.m_triggerPrice->isEmpty())
        body["triggerPrice"] = req.m_triggerPrice.value();

    return QJsonDocument(body).toJson(QJsonDocument::Compact);
}

void Core::Markets::PostBybitAmendOrderHandler::handle(const QJsonObject &data, IPrivateService *service)
{
    if (!data.contains("retMsg") || data["retMsg"].toString() != "OK")
    {
        emit service->errorOccurred(QString("Error processing request [endpoint = %1]: " + data["retMsg"].toString()).arg(endpoint()));
        return;
    }

    QJsonObject result = data["result"].toObject();
    QString orderId = result["orderId"].toString();
    QString orderLinkId = result["orderLinkId"].toString();

    emit service->orderAmended(orderId, orderLinkId);
}
