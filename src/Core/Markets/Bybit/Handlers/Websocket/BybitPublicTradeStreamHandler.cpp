#include "BybitPublicTradeStreamHandler.hpp"

void Core::Markets::BybitPublicTradeStreamHandler::handle(const QJsonObject &obj, IMarketDataStreamer *streamer)
{
    if (!obj.contains("data") || !obj["data"].isArray())
        return;

    QJsonArray arrData = obj["data"].toArray();
    QString symbol = obj["topic"].toString().section('.', -1);

    Tools::PublicTrades publicTrades{};
    publicTrades.m_symbol = symbol;

    for (const auto& val : arrData)
    {
        QJsonObject itemData = val.toObject();

        Tools::PublicTradeItem publicTradeItem{};
        publicTradeItem.m_side = itemData["S"].toString();
        publicTradeItem.m_price = itemData["p"].toString().toDouble();
        publicTradeItem.m_volume = itemData["v"].toString().toDouble();
        publicTradeItem.m_turnover = publicTradeItem.m_price * publicTradeItem.m_volume;
        publicTradeItem.m_tradeTime = itemData["T"].toVariant().toLongLong();

        publicTrades.m_items.append(publicTradeItem);
    }

    emit streamer->publicTradeUpdated(publicTrades);
}
