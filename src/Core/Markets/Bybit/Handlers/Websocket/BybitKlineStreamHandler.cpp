#include "BybitKlineStreamHandler.hpp"

void Core::Markets::BybitKlineStreamHandler::handle(const QJsonObject &obj, IMarketDataStreamer *streamer)
{
    if (!obj.contains("data") || !obj["data"].isArray())
        return;

    QJsonArray arrData = obj["data"].toArray();
    QString symbol = obj["topic"].toString().section('.', -1);

    Tools::Kline kline{};
    kline.m_symbol = symbol;

    for (const auto& val : arrData)
    {
        QJsonObject data = val.toObject();
        kline.m_open = data["open"].toString().toDouble();
        kline.m_close = data["close"].toString().toDouble();
        kline.m_high = data["high"].toString().toDouble();
        kline.m_low = data["low"].toString().toDouble();
        kline.m_confirm = data["confirm"].toBool();
        kline.m_start = data["start"].toVariant().toLongLong();
        kline.m_end = data["end"].toVariant().toLongLong();
        kline.m_interval = data["interval"].toString();
        kline.m_volume = data["volume"].toString().toDouble();
        kline.m_turnover = data["turnover"].toString().toDouble();
    }

    emit streamer->klineUpdated(kline);
}
