#pragma once
#include <QString>
#include <QList>
#include <QMap>
#include <QObject>
#include <QVariant>
#include <QtQml>

namespace Core::Tools
{
    Q_NAMESPACE

    struct AccountBalance {
        Q_GADGET
        QML_VALUE_TYPE(AccountBalance)

    public:

        double m_totalWalletBalance = 0; // Общий баланс кошелька аккаунта в USD
        std::vector<std::tuple<QString, double, double>> m_assets{}; // активы (монета, кол-во, стоимость)

    };

    using OrderbookSide = QMap<double, double>; // уровни (цена, объем)

    // Разделить на OrderbookSide и обьединить через Orderbook
    struct Orderbook {

        QString m_symbol = ""; // Название пары
        OrderbookSide m_bids{}; // Покупки
        OrderbookSide m_asks{}; // Продажи

    };

    struct Ticker {
        Q_GADGET
        QML_VALUE_TYPE(Ticker)

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

    struct ApiInfo {
        Q_GADGET
        QML_VALUE_TYPE(ApiInfo)

        Q_PROPERTY(bool readOnly MEMBER m_readOnly FINAL)
        Q_PROPERTY(bool permissionSpotTrade MEMBER m_permissionSpotTrade FINAL)
        Q_PROPERTY(bool permissionWithdraw MEMBER m_permissionWithdraw FINAL)
        Q_PROPERTY(bool permissionContractTrade MEMBER m_permissionContractTrade FINAL)
        Q_PROPERTY(bool permissionAccountTransfer MEMBER m_permissionAccountTransfer FINAL)
        Q_PROPERTY(QString expiredAt MEMBER m_expiredAt FINAL)
        Q_PROPERTY(QStringList ips MEMBER m_ips FINAL)

    public:

        QList<QString> m_ips{};                     // IP адреса, которые могут исп этот ключ
        QString m_expiredAt = "";                   // Срок истечения
        bool m_readOnly = true;                     // Только для чтения
        bool m_permissionSpotTrade = false;         // Разрешение к спотовой торговли
        bool m_permissionContractTrade = false;     // Разрешение к фьючерсаной торговли
        bool m_permissionWithdraw = false;          // Разрешение к выводу активов с биржи
        bool m_permissionAccountTransfer = false;   // Разрешение к переводу между внутренними кошельками

    };

    struct Api {
        Q_GADGET
        QML_VALUE_TYPE(Api)

        Q_PROPERTY(QString apiKey MEMBER m_apiKey FINAL)
        Q_PROPERTY(QString secretKey MEMBER m_secretKey FINAL)
        Q_PROPERTY(bool isTestnet MEMBER m_isTestnet FINAL)

    public:

        QString m_apiKey = "";              // Api ключ
        QString m_secretKey = "";           // Секретный Api ключ
        bool m_isTestnet = false;           // Тип сети

    };

    struct TradeInfo {

        QString symbol;
        QString base_coin;
        QString quote_coin;

    };

}

// РЕГИСТРАЦИЯ В МЕТАСИСТЕМЕ QT
Q_DECLARE_METATYPE(Core::Tools::AccountBalance)
Q_DECLARE_METATYPE(Core::Tools::Orderbook)
Q_DECLARE_METATYPE(Core::Tools::Ticker)
Q_DECLARE_METATYPE(Core::Tools::Kline)
Q_DECLARE_METATYPE(Core::Tools::PublicTradeItem)
Q_DECLARE_METATYPE(Core::Tools::PublicTrades)
Q_DECLARE_METATYPE(Core::Tools::Api)
Q_DECLARE_METATYPE(Core::Tools::ApiInfo)
Q_DECLARE_METATYPE(Core::Tools::TradeInfo)