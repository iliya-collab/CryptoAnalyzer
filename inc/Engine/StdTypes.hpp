#pragma once

#include <QString>
#include <QList>
#include <QMap>
#include <QObject>

namespace Engine {
    Q_NAMESPACE

    struct stOrderBooks {
        QString symbol;                 // Название пары

        QMap<double, double> bids;      // Покупки (пара цена, объем)
        QMap<double, double> asks;      // Продажи (пара цена, объем)

        quint64 lastUpdateId;
        quint64 lastSeq;
    };

    struct stTicker {
        Q_GADGET
        Q_PROPERTY(QString symbol MEMBER symbol)
        Q_PROPERTY(double lastPrice MEMBER lastPrice)
        Q_PROPERTY(double high24h MEMBER high24h)
        Q_PROPERTY(double low24h MEMBER low24h)
        Q_PROPERTY(double volCcy24h MEMBER volCcy24h)
        Q_PROPERTY(double vol24h MEMBER vol24h)
        Q_PROPERTY(double prevPrice24h MEMBER prevPrice24h)
        Q_PROPERTY(double price24hPcnt MEMBER price24hPcnt)

    public:
        double lastPrice = 0.0; // Последняя цена сделки
        double high24h = 0.0; // Максимальная цена за 24 часа
        double low24h = 0.0; // Минимальная цена за 24 часа
        double volCcy24h = 0.0; // Объем торгов в котируемой валюте за 24ч (USDT)
        double vol24h = 0.0; // Объем торгов в базовой валюте за 24ч (BTC)
        double prevPrice24h = 0.0; // Рыночная цена 24 часа назад
        double price24hPcnt = 0.0; // Процентное изменение рыночной цены относительно 24 часов
        QString symbol; // Название пары
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
