#include "BybitTickerStreamHandler.hpp"

void Core::Markets::BybitTickerStreamHandler::handle(const QJsonObject &obj, IMarketDataStreamer *streamer)
{
    if (!obj.contains("data") || !obj["data"].isObject())
    {
        emit streamer->errorOccurred(streamer->id(), "[" + topic() + "] Invalid response structure!");
        return;
    }

    QJsonObject data = obj["data"].toObject();
    QString symbol = data["symbol"].toString();

    Tools::Ticker ticker{};

    ticker.m_category = Tools::stringToMarketType(streamer->id());
    ticker.m_symbol = symbol;
    ticker.m_lastPrice = data["lastPrice"].toString().toDouble();
    ticker.m_usdIndexPrice = data["usdIndexPrice"].toString().toDouble();
    ticker.m_high24h = data["highPrice24h"].toString().toDouble();
    ticker.m_low24h = data["lowPrice24h"].toString().toDouble();
    ticker.m_volCcy24h = data["turnover24h"].toString().toDouble();
    ticker.m_vol24h = data["volume24h"].toString().toDouble();
    ticker.m_prevPrice24h = ticker.m_lastPrice - data["prevPrice24h"].toString().toDouble();
    ticker.m_price24hPcnt = data["price24hPcnt"].toString().toDouble() * 100;

    emit streamer->tickerUpdated(ticker);
}
