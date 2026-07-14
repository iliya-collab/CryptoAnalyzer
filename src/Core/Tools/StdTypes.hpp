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
        Q_GADGET
        QML_VALUE_TYPE(orderbook)

        Q_PROPERTY(QString symbol MEMBER m_symbol)
        Q_PROPERTY(QVariantList bids READ getBids)
        Q_PROPERTY(QVariantList asks READ getAsks)

    public:

        QString m_type = "";
        QString m_symbol = ""; // Название пары
        QMap<double, double> m_bids = {}; // Покупки (цена, объем)
        QMap<double, double> m_asks = {}; // Продажи (цена, объем)

        QVariantList getBids() const { return mapToVariantList(m_bids); }
        QVariantList getAsks() const { return mapToVariantList(m_asks); }

    private:
        static QVariantList mapToVariantList(const QMap<double, double>& map) {
            QVariantList list;
            for (auto it = map.constBegin(); it != map.constEnd(); ++it) {
                QVariantList pair;
                pair << it.key() << it.value();
                list.prepend(QVariant(pair));
            }
            return list;
        }

    };

    struct Ticker {
        Q_GADGET
        QML_VALUE_TYPE(ticker)

        Q_PROPERTY(QString symbol MEMBER m_symbol)
        Q_PROPERTY(double lastPrice MEMBER m_lastPrice)
        Q_PROPERTY(double usdIndexPrice MEMBER m_usdIndexPrice)
        Q_PROPERTY(double high24h MEMBER m_high24h)
        Q_PROPERTY(double low24h MEMBER m_low24h)
        Q_PROPERTY(double volCcy24h MEMBER m_volCcy24h)
        Q_PROPERTY(double vol24h MEMBER m_vol24h)
        Q_PROPERTY(double prevPrice24h MEMBER m_prevPrice24h)
        Q_PROPERTY(double price24hPcnt MEMBER m_price24hPcnt)

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
        Q_GADGET
        QML_VALUE_TYPE(kline)

        Q_PROPERTY(QString symbol MEMBER m_symbol)
        Q_PROPERTY(double open MEMBER m_open)
        Q_PROPERTY(double close MEMBER m_close)
        Q_PROPERTY(double high MEMBER m_high)
        Q_PROPERTY(double low MEMBER m_low)
        Q_PROPERTY(bool confirm MEMBER m_confirm)
        Q_PROPERTY(qint64 start MEMBER m_start)
        Q_PROPERTY(qint64 end MEMBER m_end)

    public:

        QString m_symbol = "";
        QString m_interval = "";
        double m_open = 0;
        double m_close = 0;
        double m_high = 0;
        double m_low = 0;
        qint64 m_start = 0;
        qint64 m_end = 0;
        bool m_confirm = false;

    };

    // Структура для хранения информации об API ключах
    struct API {
        Q_GADGET
        QML_VALUE_TYPE(api)

        Q_PROPERTY(QString apiKey MEMBER m_apiKey)
        Q_PROPERTY(QString secretKey MEMBER m_secretKey)
        Q_PROPERTY(bool isTestnet MEMBER m_isTestnet)

    public:

        QString m_apiKey = ""; // API ключ
        QString m_secretKey = ""; // Секретный API ключ
        bool m_isTestnet = false; // Тип сети (true - testnet, false - mainnet)
    };

}
