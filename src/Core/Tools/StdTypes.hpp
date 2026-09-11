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

// ==========================================================================================
//  Тип рынка / категория инструмента
// ==========================================================================================

enum class MarketType
{
    Spot,
    Linear,
    Inverse,
    Option,
    Unknown
};
Q_ENUM_NS(MarketType) // Регистрация для QML: Core.Tools.MarketType.Spot и т.д.

// ==========================================================================================
//  Направление / статус ордера / тип исполнения
// ==========================================================================================

// Направление операции
enum class OrderSide
{
    Buy, // Покупка. Для спота — приобретение базового актива. Для фьючерсов — открытие Long-позиции или закрытие Short-позиции.
    Sell, // Продажа. Для спота — продажа актива. Для фьючерсов — открытие Short-позиции или закрытие Long-позиции.
    Unknown // Неизвестное направление. Защитное значение на случай десериализации некорректных данных или при полном закрытии позиции (когда Bybit присылает side: "").
};
Q_ENUM_NS(OrderSide)

// Тип ордера при выставлении
enum class OrderType
{
    Limit, // Лимитный ордер — исполняется по указанной цене (m_price обязателен).
    Market, // Рыночный ордер — исполняется по текущей рыночной цене немедленно.
    Unknown
};
Q_ENUM_NS(OrderType)

enum class OrderStatus
{
    New, // Ордер успешно принят торговым движком биржи и размещен в стакане (активен).
    PartiallyFilled, // Ордер частично исполнен. Часть объема уже превратилась в сделки, но остаток (m_leavesQty) всё еще находится в стакане.
    Filled, // Ордер полностью исполнен. Весь заявленный объем (m_qty) был сведен со встречными ордерами. Ордер неактивен.
    Cancelled, // Ордер отменен пользователем или системой (например, по истечении времени действия или по правилу Post-Only), до того как был полностью исполнен.
    Rejected, // Ордер отклонен биржей на этапе валидации (например, не хватило маржи, неверная цена, сработал риск-лимит) и не попал в стакан.
    Unknown // Неизвестный статус (или редкие специфичные статусы Bybit вроде Deactivated, если они не смаппированы отдельно).
};
Q_ENUM_NS(OrderStatus)

enum class ExecType
{
    Trade, // Обычная торговая сделка на бирже (ордер сопоставился со встречным ордером в стакане).
    Adl, // Авто-делеверидж (Auto-Deleveraging). Принудительное закрытие вашей прибыльной позиции встречным ордером ликвидируемого игрока при нехватке страхового фонда биржи.
    Funding, // Периодическое списание или начисление комиссии за удержание позиций на бессрочных фьючерсах (обычно каждые 4-8 часов).
    Liquidation, // Сделка, совершенная в процессе принудительной ликвидации позиции из-за нехватки поддерживающей маржи.
    Unknown // Неизвестный тип исполнения.
};
Q_ENUM_NS(ExecType)

// Время действия ордера
enum class TimeInForce
{
    GTC, // Good Till Cancel — действует, пока не будет исполнен или отменен вручную (по умолчанию).
    IOC, // Immediate Or Cancel — исполняется немедленно (полностью или частично), неисполненный остаток отменяется.
    FOK, // Fill Or Kill — исполняется полностью и немедленно, либо отменяется целиком.
    PostOnly, // Только мейкер — ордер отклоняется, если исполнился бы сразу как тейкер.
    Unknown
};
Q_ENUM_NS(TimeInForce)

// ---- Конвертация enum <-> строка для REST-запросов и парсинга ответов ----

inline QString marketTypeToString(MarketType type)
{
    switch (type)
    {
    case MarketType::Spot:
        return "spot";
    case MarketType::Linear:
        return "linear";
    case MarketType::Inverse:
        return "inverse";
    case MarketType::Option:
        return "option";
    default:
        return "";
    }
}

inline MarketType stringToMarketType(const QString& type)
{
    if (type == "spot")
        return MarketType::Spot;
    if (type == "linear")
        return MarketType::Linear;
    if (type == "inverse")
        return MarketType::Inverse;
    if (type == "option")
        return MarketType::Option;
    return MarketType::Unknown;
}

inline QString orderSideToString(OrderSide side)
{
    switch (side)
    {
    case OrderSide::Buy:
        return "Buy";
    case OrderSide::Sell:
        return "Sell";
    default:
        return "";
    }
}

inline OrderSide stringToOrderSide(const QString& side)
{
    if (side == "Buy")
        return OrderSide::Buy;
    if (side == "Sell")
        return OrderSide::Sell;
    return OrderSide::Unknown;
}

inline QString orderStatusToString(OrderStatus status)
{
    switch (status)
    {
    case OrderStatus::New:
        return "New";
    case OrderStatus::PartiallyFilled:
        return "PartiallyFilled";
    case OrderStatus::Filled:
        return "Filled";
    case OrderStatus::Cancelled:
        return "Cancelled";
    case OrderStatus::Rejected:
        return "Rejected";
    default:
        return "";
    }
}

inline OrderStatus stringToOrderStatus(const QString& status)
{
    if (status == "New")
        return OrderStatus::New;
    if (status == "PartiallyFilled")
        return OrderStatus::PartiallyFilled;
    if (status == "Filled")
        return OrderStatus::Filled;
    if (status == "Cancelled")
        return OrderStatus::Cancelled;
    if (status == "Rejected")
        return OrderStatus::Rejected;
    return OrderStatus::Unknown;
}

inline QString execTypeToString(ExecType type)
{
    switch (type)
    {
    case ExecType::Trade:
        return "Trade";
    case ExecType::Adl:
        return "Adl";
    case ExecType::Funding:
        return "Funding";
    case ExecType::Liquidation:
        return "Liquidation";
    default:
        return "";
    }
}

inline ExecType stringToExecType(const QString& type)
{
    if (type == "Trade")
        return ExecType::Trade;
    if (type == "Adl")
        return ExecType::Adl;
    if (type == "Funding")
        return ExecType::Funding;
    if (type == "Liquidation")
        return ExecType::Liquidation;
    return ExecType::Unknown;
}

inline QString orderTypeToString(OrderType type)
{
    switch (type)
    {
    case OrderType::Limit:
        return "Limit";
    case OrderType::Market:
        return "Market";
    default:
        return "";
    }
}

inline OrderType stringToOrderType(const QString& type)
{
    if (type == "Limit")
        return OrderType::Limit;
    if (type == "Market")
        return OrderType::Market;
    return OrderType::Unknown;
}

inline QString timeInForceToString(TimeInForce tif)
{
    switch (tif)
    {
    case TimeInForce::GTC:
        return "GTC";
    case TimeInForce::IOC:
        return "IOC";
    case TimeInForce::FOK:
        return "FOK";
    case TimeInForce::PostOnly:
        return "PostOnly";
    default:
        return "";
    }
}

inline TimeInForce stringToTimeInForce(const QString& tif)
{
    if (tif == "GTC")
        return TimeInForce::GTC;
    if (tif == "IOC")
        return TimeInForce::IOC;
    if (tif == "FOK")
        return TimeInForce::FOK;
    if (tif == "PostOnly")
        return TimeInForce::PostOnly;
    return TimeInForce::Unknown;
}

// ==========================================================================================
//  Баланс аккаунта
// ==========================================================================================

struct AccountBalance {
    Q_GADGET
    QML_VALUE_TYPE(AccountBalance)

public:

    double m_totalWalletBalance = 0; // Общий баланс кошелька аккаунта в USD
    std::vector<std::tuple<QString, double, double>> m_assets{}; // активы (монета, кол-во, стоимость)

};

// ==========================================================================================
//  Стакан заявок
// ==========================================================================================

using OrderbookSide = QMap<double, double>; // уровни (цена, объем)

struct Orderbook {

    MarketType m_category = MarketType::Unknown;
    QString m_symbol = ""; // Название пары
    OrderbookSide m_bids{}; // Покупки
    OrderbookSide m_asks{}; // Продажи

};

// ==========================================================================================
//  Тикер
// ==========================================================================================

struct Ticker {
    Q_GADGET
    QML_VALUE_TYPE(Ticker)

    Q_PROPERTY(Core::Tools::MarketType category MEMBER m_category FINAL)
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

    MarketType m_category = MarketType::Unknown;
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

// ==========================================================================================
//  Свеча
// ==========================================================================================

struct Kline {

    MarketType m_category = MarketType::Unknown;
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

// ==========================================================================================
//  Публичная лента сделок
// ==========================================================================================

struct PublicTradeItem {

    OrderSide m_side = OrderSide::Unknown; // Сторона инициатора сделки
    double m_price = 0;
    double m_volume = 0;
    double m_turnover = 0;
    qint64 m_tradeTime = 0;

};

struct PublicTrades {

    MarketType m_category = MarketType::Unknown;
    QString m_symbol = "";
    QList<PublicTradeItem> m_items{};

};

// ==========================================================================================
//  API-ключ и информация о нём
// ==========================================================================================

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

// ==========================================================================================
//  Торговая пара как инструмент
// ==========================================================================================

struct TradeInfo {

    MarketType m_category = MarketType::Unknown;
    QString m_symbol = "";
    QString m_baseCoin = "";
    QString m_quoteCoin = "";

};

// ==========================================================================================
//  Ордер / исполнение / позиция (приватный WS-стрим)
// ==========================================================================================

// Информация об ордере
struct OrderInfo
{
    Q_GADGET
    QML_VALUE_TYPE(OrderInfo)

    Q_PROPERTY(Core::Tools::MarketType category MEMBER m_category FINAL)
    Q_PROPERTY(QString orderId MEMBER m_orderId FINAL)
    Q_PROPERTY(QString orderLinkId MEMBER m_orderLinkId FINAL)
    Q_PROPERTY(QString symbol MEMBER m_symbol FINAL)
    Q_PROPERTY(Core::Tools::OrderSide side MEMBER m_side FINAL)
    Q_PROPERTY(Core::Tools::OrderType orderType MEMBER m_orderType FINAL)
    Q_PROPERTY(double price MEMBER m_price FINAL)
    Q_PROPERTY(double qty MEMBER m_qty FINAL)
    Q_PROPERTY(Core::Tools::OrderStatus status MEMBER m_status FINAL)
    Q_PROPERTY(int positionIdx MEMBER m_positionIdx FINAL)
    Q_PROPERTY(double leavesQty MEMBER m_leavesQty FINAL)
    Q_PROPERTY(double cumExecQty MEMBER m_cumExecQty FINAL)
    Q_PROPERTY(double cumExecValue MEMBER m_cumExecValue FINAL)
    Q_PROPERTY(double cumExecFee MEMBER m_cumExecFee FINAL)
    Q_PROPERTY(QString rejectReason MEMBER m_rejectReason FINAL)
    Q_PROPERTY(qint64 createdTime MEMBER m_createdTime FINAL)
    Q_PROPERTY(qint64 updatedTime MEMBER m_updatedTime FINAL)

public:

    MarketType m_category = MarketType::Unknown; // Категория рынка
    QString m_orderId; // Уникальный идентификатор ордера, присвоенный биржей Bybit
    QString m_orderLinkId; // Клиентский идентификатор ордера (задается вами при отправке для локального трекинга).
    QString m_symbol; // Торговая пара (например, "BTCUSDT").
    OrderSide m_side = OrderSide::Unknown; // Направление ордера (Buy или Sell).
    OrderType m_orderType; // Тип ордера (Limit, Market).
    double m_price = 0.0; // Цена, указанная при выставлении ордера (для рыночных ордеров обычно 0.0 или цена скольжения).
    double m_qty = 0.0; // Изначальный (заявленный) объем ордера в базовой валюте.
    OrderStatus m_status = OrderStatus::Unknown; // Текущий статус ордера (New, Filled и т.д.).
    int m_positionIdx = 0; // Режим позиции (0-OneWay, 1-Hedge Long, 2-Hedge Short); актуально для деривативов в хедж-режиме.
    double m_leavesQty = 0.0; // Сколько еще осталось исполнить.
    double m_cumExecQty = 0.0; // Сколько суммарно уже исполнено.
    double m_cumExecValue = 0.0; // Суммарная стоимость исполненной части.
    double m_cumExecFee = 0.0; // Накопленная комиссия по ордеру.
    QString m_rejectReason; // Причина отклонения, если статус Rejected.
    qint64 m_createdTime = 0; // Таймстемп создания (мс).
    qint64 m_updatedTime = 0; // Таймстемп последнего изменения (мс).
};

struct ExecutionInfo
{
    Q_GADGET
    QML_VALUE_TYPE(ExecutionInfo)

    Q_PROPERTY(Core::Tools::MarketType category MEMBER m_category FINAL)
    Q_PROPERTY(QString symbol MEMBER m_symbol FINAL)
    Q_PROPERTY(QString execId MEMBER m_execId FINAL)
    Q_PROPERTY(QString orderId MEMBER m_orderId FINAL)
    Q_PROPERTY(QString orderLinkId MEMBER m_orderLinkId FINAL)
    Q_PROPERTY(Core::Tools::OrderSide side MEMBER m_side FINAL)
    Q_PROPERTY(double execPrice MEMBER m_execPrice FINAL)
    Q_PROPERTY(double execQty MEMBER m_execQty FINAL)
    Q_PROPERTY(double execValue MEMBER m_execValue FINAL)
    Q_PROPERTY(Core::Tools::ExecType execType MEMBER m_execType FINAL)
    Q_PROPERTY(double execFee MEMBER m_execFee FINAL)
    Q_PROPERTY(QString feeCurrency MEMBER m_feeCurrency FINAL)
    Q_PROPERTY(bool isMaker MEMBER m_isMaker FINAL)
    Q_PROPERTY(qint64 execTime MEMBER m_execTime FINAL)

public:

    MarketType m_category = MarketType::Unknown; // Категория рынка
    QString m_symbol; // Торговая пара
    QString m_execId; // Уникальный ID транзакции (филла)
    QString m_orderId; // ID родительского ордера
    QString m_orderLinkId; // Ваш клиентский ID
    OrderSide m_side = OrderSide::Unknown; // Buy, Sell

    double m_execPrice = 0.0; // Цена этой конкретной сделки
    double m_execQty = 0.0; // Объем этой конкретной сделки
    double m_execValue = 0.0; // Стоимость этой конкретной сделки

    ExecType m_execType = ExecType::Unknown; // Trade, Adl, Funding, Liquidation
    double m_execFee = 0.0; // Комиссия за эту сделку (отрицательная = рибейт)
    QString m_feeCurrency; // Валюта комиссии (USDT, BTC и т.д.)
    bool m_isMaker = false; // true = Maker, false = Taker
    qint64 m_execTime = 0; // Время совершения сделки (мс)
};

struct PositionInfo
{
    Q_GADGET
    QML_VALUE_TYPE(PositionInfo)

    Q_PROPERTY(Core::Tools::MarketType category MEMBER m_category FINAL)
    Q_PROPERTY(QString symbol MEMBER m_symbol FINAL)
    Q_PROPERTY(Core::Tools::OrderSide side MEMBER m_side FINAL)
    Q_PROPERTY(double size MEMBER m_size FINAL)
    Q_PROPERTY(int positionIdx MEMBER m_positionIdx FINAL)
    Q_PROPERTY(double avgPrice MEMBER m_avgPrice FINAL)
    Q_PROPERTY(double liqPrice MEMBER m_liqPrice FINAL)
    Q_PROPERTY(double markPrice MEMBER m_markPrice FINAL)
    Q_PROPERTY(double leverage MEMBER m_leverage FINAL)
    Q_PROPERTY(double positionIM MEMBER m_positionIM FINAL)
    Q_PROPERTY(double positionMM MEMBER m_positionMM FINAL)
    Q_PROPERTY(double unrealisedPnl MEMBER m_unrealisedPnl FINAL)
    Q_PROPERTY(double cumRealisedPnl MEMBER m_cumRealisedPnl FINAL)
    Q_PROPERTY(int adlRankIndicator MEMBER m_adlRankIndicator FINAL)

public:

    MarketType m_category = MarketType::Unknown; // Категория рынка (linear, inverse)
    QString m_symbol; // Торговая пара
    OrderSide m_side = OrderSide::Unknown; // Buy=Long, Sell=Short, Unknown=нет позиции
    double m_size = 0.0; // Текущий размер открытой позиции (0 если закрыта)
    int m_positionIdx = 0; // Режим: 0-OneWay, 1-Hedge Long, 2-Hedge Short

    // Финансовые метрики риска
    double m_avgPrice = 0.0; // Средняя цена входа
    double m_liqPrice = 0.0; // Цена ликвидации
    double m_markPrice = 0.0; // Текущая цена маркировки
    double m_leverage = 0.0; // Плечо
    double m_positionIM = 0.0; // Начальная маржа под позицию
    double m_positionMM = 0.0; // Поддерживающая маржа

    // Результат
    double m_unrealisedPnl = 0.0; // Нереализованный PnL
    double m_cumRealisedPnl = 0.0; // Накопленный реализованный PnL

    int m_adlRankIndicator = 0; // Очередь на авто-делеверидж (0-5)
};

// ==========================================================================================
//  Параметры REST-запросов управления ордерами
// ==========================================================================================

// Параметры создания нового ордера.
struct OrderRequest
{
    MarketType m_category = MarketType::Unknown; // Обязательно: spot, linear, inverse, option
    QString m_symbol; // Обязательно: торговая пара, например "BTCUSDT"
    OrderSide m_side = OrderSide::Unknown; // Обязательно: Buy или Sell
    OrderType m_orderType = OrderType::Unknown; // Обязательно: Limit или Market
    QString m_qty; // Обязательно: объем ордера (строкой, как того требует API)

    std::optional<QString> m_price; // Обязателен для Limit; для Market обычно не передается
    std::optional<QString> m_orderLinkId; // Опционально: клиентский ID для локального трекинга
    std::optional<TimeInForce> m_timeInForce = TimeInForce::GTC; // Опционально: GTC (по умолчанию), IOC, FOK, PostOnly
    std::optional<QString> m_takeProfit; // Опционально: цена тейк-профита
    std::optional<QString> m_stopLoss; // Опционально: цена стоп-лосса
    std::optional<QString> m_triggerPrice; // Опционально: триггерная цена для условных (стоп) ордеров
    std::optional<bool> m_reduceOnly = false; // Опционально: ордер только на закрытие позиции (для деривативов)
};

// Параметры изменения существующего ордера.
struct OrderAmendRequest
{
    MarketType m_category = MarketType::Unknown; // Обязательно
    QString m_symbol; // Обязательно
    std::optional<QString> m_orderId; // Обязательно: orderId ИЛИ orderLinkId — нужен хотя бы один
    std::optional<QString> m_orderLinkId;

    std::optional<QString> m_qty; // Новое количество (опционально)
    std::optional<QString> m_price; // Новая цена (опционально)
    std::optional<QString> m_triggerPrice; // Новая триггерная цена для стоп-ордеров (опционально)
    std::optional<QString> m_takeProfit; // Новый уровень TP, "0" — удалить (опционально)
    std::optional<QString> m_stopLoss; // Новый уровень SL, "0" — удалить (опционально)
};

// Oтмена одного конкретного ордера.
struct OrderCancelRequest
{
    MarketType m_category = MarketType::Unknown; // Обязательно
    QString m_symbol; // Обязательно
    std::optional<QString> m_orderId; // Обязательно: orderId ИЛИ orderLinkId — нужен хотя бы один
    std::optional<QString> m_orderLinkId;
};

// Массовая отмена ордеров.
// Для деривативов обязателен один из трех фильтров: symbol, baseCoin или settleCoin.
struct OrderCancelAllRequest
{
    MarketType m_category = MarketType::Unknown; // Обязательно

    std::optional<QString> m_symbol; // Высший приоритет фильтрации, если задан
    std::optional<QString> m_baseCoin; // Например "BTC"
    std::optional<QString> m_settleCoin; // Например "USDT"
};

// Запрос активных (и опционально недавних финальных) ордеров.
struct OpenOrdersRequest
{
    MarketType m_category = MarketType::Unknown; // Обязательно

    std::optional<QString> m_symbol; // Опционально: фильтр по паре
    std::optional<QString> m_orderId; // Опционально: поиск конкретного ордера
    std::optional<QString> m_orderLinkId;
    std::optional<bool> m_openOnly = false; // false (0) — только открытые (по умолчанию); true (1) — из последних 500 в финальном статусе
};

// Запрос истории закрытых/отмененных/исполненных ордеров (архив до 2 лет).
struct OrderHistoryRequest
{
    MarketType m_category = MarketType::Unknown; // Обязательно

    std::optional<QString> m_symbol; // Опционально: фильтр по паре
    std::optional<QString> m_orderId; // Опционально: поиск конкретного ордера
    std::optional<QString> m_orderLinkId;
    std::optional<qint64> m_startTime = 0; // Опционально: начало диапазона (мс)
    std::optional<qint64> m_endTime = 0; // Опционально: конец диапазона (мс)
    std::optional<int> m_limit = 50; // Опционально: записей на страницу (обычно до 50-100)
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
Q_DECLARE_METATYPE(Core::Tools::OrderInfo)
Q_DECLARE_METATYPE(Core::Tools::ExecutionInfo)
Q_DECLARE_METATYPE(Core::Tools::PositionInfo)
Q_DECLARE_METATYPE(Core::Tools::MarketType)
Q_DECLARE_METATYPE(Core::Tools::OrderType)
Q_DECLARE_METATYPE(Core::Tools::OrderSide)
Q_DECLARE_METATYPE(Core::Tools::OrderStatus)
Q_DECLARE_METATYPE(Core::Tools::ExecType)
Q_DECLARE_METATYPE(Core::Tools::OrderRequest)
Q_DECLARE_METATYPE(Core::Tools::OrderAmendRequest)
Q_DECLARE_METATYPE(Core::Tools::OrderCancelRequest)
Q_DECLARE_METATYPE(Core::Tools::OrderCancelAllRequest)
Q_DECLARE_METATYPE(Core::Tools::OpenOrdersRequest)
Q_DECLARE_METATYPE(Core::Tools::OrderHistoryRequest)
