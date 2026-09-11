#include "BybitExecutionStreamHandler.hpp"

void Core::Markets::BybitExecutionStreamHandler::handle(const QJsonObject &data, IPrivateStreamer *streamer)
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

        Tools::ExecutionInfo exec{};

        exec.m_category    = Tools::stringToMarketType(dataObj["category"].toString());
        exec.m_symbol      = dataObj["symbol"].toString();
        exec.m_execId      = dataObj["execId"].toString();
        exec.m_orderId     = dataObj["orderId"].toString();
        exec.m_orderLinkId = dataObj["orderLinkId"].toString();
        exec.m_feeCurrency = dataObj["feeCurrency"].toString();

        QString strSide = dataObj["side"].toString();
        if (strSide == "Buy")
            exec.m_side = Tools::OrderSide::Buy;
        else if (strSide == "Sell")
            exec.m_side = Tools::OrderSide::Sell;
        else
            exec.m_side = Tools::OrderSide::Unknown;

        QString strExecType = dataObj["execType"].toString();
        if (strExecType == "Trade")
            exec.m_execType = Tools::ExecType::Trade;
        else if (strExecType == "Adl")
            exec.m_execType = Tools::ExecType::Adl;
        else if (strExecType == "Funding")
            exec.m_execType = Tools::ExecType::Funding;
        else if (strExecType == "Liquidation" || strExecType == "BustTrade")
            exec.m_execType = Tools::ExecType::Liquidation;
        else
            exec.m_execType = Tools::ExecType::Unknown;

        exec.m_execPrice   = dataObj["execPrice"].toString().toDouble();
        exec.m_execQty     = dataObj["execQty"].toString().toDouble();
        exec.m_execValue   = dataObj["execValue"].toString().toDouble();
        exec.m_execFee     = dataObj["execFee"].toString().toDouble();

        exec.m_isMaker     = dataObj["isMaker"].toBool();
        exec.m_execTime    = dataObj["execTime"].toString().toLongLong();

        emit streamer->executionUpdated(exec);
    }
}
