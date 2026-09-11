#include "BybitPublicTradeStreamHandler.hpp"

void Core::Markets::BybitPublicTradeStreamHandler::handle(const QJsonObject &obj, IMarketDataStreamer *streamer)
{
    if (!obj.contains("data") || !obj["data"].isArray())
    {
        emit streamer->errorOccurred(streamer->id(), "[" + topic() + "] Invalid response structure!");
        return;
    }

    QJsonArray arrData = obj["data"].toArray();
    QString symbol = obj["topic"].toString().section('.', -1);

    Tools::PublicTrades publicTrades{};
    publicTrades.m_category = Tools::stringToMarketType(streamer->id());
    publicTrades.m_symbol = symbol;

    for (const auto& val : std::as_const(arrData))
    {
        QJsonObject itemData = val.toObject();

        Tools::PublicTradeItem publicTradeItem{};
        publicTradeItem.m_side = Tools::stringToOrderSide(itemData["S"].toString());
        publicTradeItem.m_price = itemData["p"].toString().toDouble();
        publicTradeItem.m_volume = itemData["v"].toString().toDouble();
        publicTradeItem.m_turnover = publicTradeItem.m_price * publicTradeItem.m_volume;
        publicTradeItem.m_tradeTime = itemData["T"].toVariant().toLongLong();

        publicTrades.m_items.append(publicTradeItem);
    }

    emit streamer->publicTradeUpdated(publicTrades);
}
