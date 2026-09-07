#include "BybitPositionStreamHandler.hpp"

void Core::Markets::BybitPositionStreamHandler::handle(const QJsonObject &data, IPrivateMarketDataStreamer *streamer)
{
    if (!data.contains("data") || !data["data"].isArray())
    {
        emit streamer->errorOccurred(streamer->id(), "[Position] Invalid response structure!");
        return;
    }

    QJsonArray dataArray = data["data"].toArray();
    if (dataArray.isEmpty())
        return;

    for (const auto& iData : std::as_const(dataArray))
    {
        QJsonObject dataObj = iData.toObject();

        Tools::PositionInfo position{};

        position.m_category = Tools::stringToMarketType(dataObj["category"].toString());
        position.m_symbol = dataObj["symbol"].toString();
        QString strSide = dataObj["side"].toString();
        if (strSide == "Buy")
            position.m_side = Tools::OrderSide::Buy;
        else if (strSide == "Sell")
            position.m_side = Tools::OrderSide::Sell;
        else
        {
            if (position.m_positionIdx == 1)
                position.m_side = Tools::OrderSide::Buy;
            else if (position.m_positionIdx == 2)
                position.m_side = Tools::OrderSide::Sell;
            else
                position.m_side = Tools::OrderSide::Unknown;
        }
        position.m_size = dataObj["size"].toString().toDouble();
        position.m_positionIdx = dataObj["positionIdx"].toInt();

        position.m_avgPrice          = dataObj["entryPrice"].toString().toDouble();
        position.m_liqPrice          = dataObj["liqPrice"].toString().toDouble();
        position.m_markPrice         = dataObj["markPrice"].toString().toDouble();
        position.m_leverage          = dataObj["leverage"].toString().toDouble();
        position.m_positionIM        = dataObj["positionIM"].toString().toDouble();
        position.m_positionMM        = dataObj["positionMM"].toString().toDouble();
        position.m_unrealisedPnl     = dataObj["unrealisedPnl"].toString().toDouble();
        position.m_cumRealisedPnl    = dataObj["cumRealisedPnl"].toString().toDouble();

        position.m_adlRankIndicator  = dataObj["adlRankIndicator"].toInt();

        emit streamer->positionUpdated(position);
    }
}
