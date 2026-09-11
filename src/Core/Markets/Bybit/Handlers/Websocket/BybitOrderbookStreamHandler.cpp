#include "BybitOrderbookStreamHandler.hpp"

void Core::Markets::BybitOrderbookStreamHandler::handle(const QJsonObject &obj, IMarketDataStreamer *streamer)
{
    qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

    if (!obj.contains("data") || !obj["data"].isObject())
    {
        emit streamer->errorOccurred(streamer->id(), "[" + topic() + "] Invalid response structure!");
        return;
    }

    QJsonObject data = obj["data"].toObject();
    QString symbol = data["s"].toString();
    QString type = obj["type"].toString();

    Tools::Orderbook orderbook{};

    orderbook.m_symbol = symbol;

    QJsonArray bidsArray = data.value("b").toArray();
    for (const auto& bidVal : std::as_const(bidsArray))
    {
        QJsonArray bid = bidVal.toArray();
        double price = bid[0].toString().toDouble();
        double size = bid[1].toString().toDouble();
        orderbook.m_bids.insert(price, size);
    }

    QJsonArray asksArray = data.value("a").toArray();
    for (const auto& askVal : std::as_const(asksArray))
    {
        QJsonArray ask = askVal.toArray();
        double price = ask[0].toString().toDouble();
        double size = ask[1].toString().toDouble();
        orderbook.m_asks.insert(price, size);
    }

    m_savedOrderbook.m_category = Tools::stringToMarketType(streamer->id());

    if (type == "snapshot")
        snapshotOrderbook(m_savedOrderbook, orderbook);
    else
        deltaUpdateOrderbook(m_savedOrderbook, orderbook);

    emit streamer->orderbookUpdated(m_savedOrderbook);
}


void Core::Markets::BybitOrderbookStreamHandler::snapshotOrderbook(Tools::Orderbook& oldOrderbook, const Tools::Orderbook& newOrderbook)
{
    oldOrderbook.m_bids = newOrderbook.m_bids;
    oldOrderbook.m_asks = newOrderbook.m_asks;
}

void Core::Markets::BybitOrderbookStreamHandler::deltaUpdateOrderbook(Tools::Orderbook& oldOrderbook, const Tools::Orderbook& newOrderbook)
{
    for (const auto& bidVal : newOrderbook.m_bids.asKeyValueRange())
    {
        double price = bidVal.first;
        double size = bidVal.second;

        if (qFuzzyIsNull(size) || size <= 0)
            oldOrderbook.m_bids.remove(price);
        else
            oldOrderbook.m_bids.insert(price, size);
    }

    for (const auto& askVal : newOrderbook.m_asks.asKeyValueRange())
    {
        double price = askVal.first;
        double size = askVal.second;

        if (qFuzzyIsNull(size) || size <= 0)
            oldOrderbook.m_asks.remove(price);
        else
            oldOrderbook.m_asks.insert(price, size);
    }
}