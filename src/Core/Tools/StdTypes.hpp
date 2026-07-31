#pragma once
#include <QString>
#include <QList>
#include <QMap>
#include <QObject>
#include <QVariant>
#include <QtQml>

namespace Core::Tools {
    Q_NAMESPACE

    struct Orderbook {

        QString m_type = "";
        QString m_symbol = ""; // Название пары
        QMap<double, double> m_bids{}; // Покупки (цена, объем)
        QMap<double, double> m_asks{}; // Продажи (цена, объем)

    };

    struct Ticker {
        Q_GADGET
        QML_VALUE_TYPE(ticker)

        Q_PROPERTY(QString symbol MEMBER m_symbol FINAL)
        Q_PROPERTY(double lastPrice MEMBER m_lastPrice FINAL)
        Q_PROPERTY(double usdIndexPrice MEMBER m_usdIndexPrice FINAL)
        Q_PROPERTY(double high24h MEMBER m_high24h FINAL)
        Q_PROPERTY(double low24h MEMBER m_low24h FINAL)
        Q_PROPERTY(double volCcy24h MEMBER m_volCcy24h FINAL)
        Q_PROPERTY(double vol24h MEMBER m_vol24h FINAL)
        Q_PROPERTY(double prevPrice24h MEMBER m_prevPrice24h FINAL)
        Q_PROPERTY(double price24hPcnt MEMBER m_price24hPcnt FINAL)

    public:

        QString m_symbol = ""; // Название пары
        double m_lastPrice = 0.0; // Последняя цена сделки
        double m_usdIndexPrice = 0; // Индексная цена доллара США
        double m_high24h = 0.0; // Максимальная цена за 24 часа
        double m_low24h = 0.0; // Минимальная цена за 24 часа
        double m_volCcy24h = 0.0; // Объем торгов в котируемой валюте за 24ч (USDT)
        double m_vol24h = 0.0; // Объем торгов в базовой валюте за 24ч (BTC)
        double m_prevPrice24h = 0.0; // Рыночная цена 24 часа назад
        double m_price24hPcnt = 0.0; // Процентное изменение рыночной цены относительно 24 часов

    };

    struct Kline {

        QString m_symbol = "";
        QString m_interval = "";
        double m_open = 0;
        double m_close = 0;
        double m_high = 0;
        double m_low = 0;
        qint64 m_start = 0;
        qint64 m_end = 0;
        double m_volume = 0;
        double m_turnover = 0;
        bool m_confirm = false;

    };

    struct PublicTradeItem {

        QString m_side = "";
        double m_price = 0;
        double m_volume = 0;
        double m_turnover = 0;
        qint64 m_tradeTime = 0;


    };

    struct PublicTrades {

        QString m_symbol = "";
        QList<PublicTradeItem> m_items{};

    };

    struct API {
        Q_GADGET
        QML_VALUE_TYPE(api)

        Q_PROPERTY(QString apiKey MEMBER m_apiKey FINAL)
        Q_PROPERTY(QString secretKey MEMBER m_secretKey FINAL)
        Q_PROPERTY(bool isTestnet MEMBER m_isTestnet FINAL)

    public:

        QString m_apiKey = ""; // API ключ
        QString m_secretKey = ""; // Секретный API ключ
        bool m_isTestnet = false; // Тип сети (true - testnet, false - mainnet)
    };

}
