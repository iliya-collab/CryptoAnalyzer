#include "BybitOrderStreamHandler.hpp"

void Core::Markets::BybitOrderStreamHandler::handle(const QJsonObject &data, IPrivateStreamer *streamer)
{
    if (!data.contains("data") || !data["data"].isArray())
    {
        emit streamer->errorOccurred(streamer->id(), "[" + topic() + "] Invalid response structure!");
        return;
    }

    QJsonArray dataArray = data["data"].toArray();
    if (dataArray.isEmpty())
        return;

    for (const auto& iData : std::as_const(dataArray))
    {
        QJsonObject dataObj = iData.toObject();

        Tools::OrderInfo order{};

        order.m_category    = Tools::stringToMarketType(dataObj["category"].toString());
        order.m_orderId     = dataObj["orderId"].toString();
        order.m_orderLinkId = dataObj["orderLinkId"].toString();
        order.m_symbol      = dataObj["symbol"].toString();
        order.m_orderType   = Tools::stringToOrderType(dataObj["orderType"].toString());

        QString strSide = dataObj["side"].toString();
        if (strSide == "Buy")
            order.m_side = Tools::OrderSide::Buy;
        else if (strSide == "Sell")
            order.m_side = Tools::OrderSide::Sell;
        else
            order.m_side = Tools::OrderSide::Unknown;

        QString strStatus = dataObj["orderStatus"].toString();
        if (strStatus == "New")
            order.m_status = Tools::OrderStatus::New;
        else if (strStatus == "PartiallyFilled")
            order.m_status = Tools::OrderStatus::PartiallyFilled;
        else if (strStatus == "Filled")
            order.m_status = Tools::OrderStatus::Filled;
        else if (strStatus == "Cancelled" || strStatus == "Deactivated")
            order.m_status = Tools::OrderStatus::Cancelled;
        else if (strStatus == "Rejected")
            order.m_status = Tools::OrderStatus::Rejected;
        else
            order.m_status = Tools::OrderStatus::Unknown;

        order.m_price        = dataObj["price"].toString().toDouble();
        order.m_qty          = dataObj["qty"].toString().toDouble();
        order.m_leavesQty    = dataObj["leavesQty"].toString().toDouble();
        order.m_cumExecQty   = dataObj["cumExecQty"].toString().toDouble();
        order.m_cumExecValue = dataObj["cumExecValue"].toString().toDouble();
        order.m_cumExecFee   = dataObj["cumExecFee"].toString().toDouble();

        order.m_rejectReason = dataObj["rejectReason"].toString();
        order.m_createdTime  = dataObj["createdTime"].toString().toLongLong();
        order.m_updatedTime  = dataObj["updatedTime"].toString().toLongLong();

        order.m_positionIdx  = dataObj["positionIdx"].toVariant().toInt();

        emit streamer->orderUpdated(order);
    }
}
