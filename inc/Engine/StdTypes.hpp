#pragma once

#include <QString>
#include <QList>

namespace Engine {

    struct Ask {
        double askPrice;                // Цена продажи
        double askSize;                 // Объем продажи 
    };

    struct Bid {
        double bidPrice;                // Цена покупки
        double bidSize;                 // Объем покупки
    };

    struct stOrderBooks {
        QString namePair;               // Название монетной пары (Binance/spot:BTCUSDT)

        QList<Bid> bids;                // Покупки
        QList<Ask> asks;                // Продажи

        double spread;                  // Спред

        double totalBidVolume;          // Общий обЪем покупки
        double totalAskVolume;          // Общий обЪем продажи
    };

    struct stTicker {
        QString namePair;               // Название монетной пары (BTCUSDT)

        double curPrice;                // Последняя цена сделки

        double high24h;                 // Максимальная цена за 24 часа
        double low24h;                  // Минимальная цена за 24 часа
        double volCcy24h;               // Объем торгов в котируемой валюте за 24ч (USDT)
        double vol24h;                  // Объем торгов в базовой валюте за 24ч (BTC)

        Ask bestAsk;                    // Лучшая продажа
        Bid bestBid;                    // Лучшая покупка
        double spread;                  // Спред
    };

    enum TMarketData {
        SPOT,
        FUTURES
    };

} // namespace Engine
