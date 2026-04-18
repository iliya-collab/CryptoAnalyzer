#pragma once

#include <QString>
#include <QList>
#include <QMap>
#include <QObject>

namespace Engine {
    Q_NAMESPACE

    struct stOrderBooks {
        QString symbol;                 // Название монетной пары (Binance/spot:BTCUSDT)

        QMap<double, double> bids;      // Покупки (пара цена, объем)
        QMap<double, double> asks;      // Продажи (пара цена, объем)

        quint64 lastUpdateId;
        quint64 lastSeq;
    };

    struct stTicker {
        QString symbol;                 // Название монетной пары

        double lastPrice;               // Последняя цена сделки

        double high24h;                 // Максимальная цена за 24 часа
        double low24h;                  // Минимальная цена за 24 часа
        double volCcy24h;               // Объем торгов в котируемой валюте за 24ч (USDT)
        double vol24h;                  // Объем торгов в базовой валюте за 24ч (BTC)
    };

    enum class TypeTrade {
        SPOT,                           // Спот
        LINEAR,                         // Бессрочные фьючерсы (USDT или USDC)
        INVERSE,                        // Инверсные фьючерсы
        OPTION                          // Опционы
    };
    Q_ENUM_NS(TypeTrade)

    QString tradeToString(TypeTrade t_trade);

    QString tradeToBaseEndpoint(TypeTrade t_trade);

    struct TradingInfo {
        QString symbol;
        QString base_coin;
        QString quote_coin;
        QString category;
    };

} // namespace Engine
