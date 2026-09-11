#include "PostBybitCreateOrderHandler.hpp"

QByteArray Core::Markets::PostBybitCreateOrderHandler::buildRequestBody(const Tools::OrderRequest &req)
{
    QJsonObject body;

    body["category"] = Tools::marketTypeToString(req.m_category);
    body["symbol"] = req.m_symbol;
    body["side"] = Tools::orderSideToString(req.m_side);
    body["orderType"] = Tools::orderTypeToString(req.m_orderType);
    body["qty"] = req.m_qty;

    if (req.m_price.has_value() && !req.m_price->isEmpty())
        body["price"] = req.m_price.value();
    if (req.m_orderLinkId.has_value() && !req.m_orderLinkId->isEmpty())
        body["orderLinkId"] = req.m_orderLinkId.value();
    if (req.m_timeInForce.has_value())
        body["timeInForce"] = Tools::timeInForceToString(req.m_timeInForce.value());
    if (req.m_takeProfit.has_value() && !req.m_takeProfit->isEmpty())
        body["takeProfit"] = req.m_takeProfit.value();
    if (req.m_stopLoss.has_value() && !req.m_stopLoss->isEmpty())
        body["stopLoss"] = req.m_stopLoss.value();
    if (req.m_triggerPrice.has_value() && !req.m_triggerPrice->isEmpty())
        body["triggerPrice"] = req.m_triggerPrice.value();
    if (req.m_reduceOnly.has_value())
        body["reduceOnly"] = req.m_reduceOnly.value();

    return QJsonDocument(body).toJson(QJsonDocument::Compact);
}

void Core::Markets::PostBybitCreateOrderHandler::handle(const QJsonObject &data, IPrivateService *service)
{
    if (!data.contains("retMsg") || data["retMsg"].toString() != "OK")
    {
        emit service->errorOccurred(QString("Error processing request [endpoint = %1]: " + data["retMsg"].toString()).arg(endpoint()));
        return;
    }

    QJsonObject result = data["result"].toObject();
    QString orderId = result["orderId"].toString();
    QString orderLinkId = result["orderLinkId"].toString();

    emit service->orderCreated(orderId, orderLinkId);
}
