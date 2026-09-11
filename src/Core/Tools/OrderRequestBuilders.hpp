#pragma once
#include "StdTypes.hpp"
#include <QVariantMap>
#include <optional>
#include <expected>

namespace Core::Tools
{

// ==========================================================================================
//  Сборка структур-запросов из QVariantMap (JS-объекта, пришедшего из QML) с валидацией.
// ==========================================================================================

    namespace detail
    {

        inline std::optional<QString> optString(const QVariantMap& map, const QString& key)
        {
            auto it = map.constFind(key);
            if (it == map.constEnd())
                return std::nullopt;
            return it->toString();
        }

        inline std::optional<bool> optBool(const QVariantMap& map, const QString& key)
        {
            auto it = map.constFind(key);
            if (it == map.constEnd())
                return std::nullopt;
            return it->toBool();
        }

        inline std::optional<qint64> optInt64(const QVariantMap& map, const QString& key)
        {
            auto it = map.constFind(key);
            if (it == map.constEnd())
                return std::nullopt;
            return it->toLongLong();
        }

        inline QString reqString(const QVariantMap& map, const QString& key)
        {
            return map.value(key).toString();
        }

        inline bool hasEitherId(std::optional<QString> orderId, std::optional<QString> orderLinkId)
        {
            return (orderId.has_value() && !orderId->isEmpty()) ||
                   (orderLinkId.has_value() && !orderLinkId->isEmpty());
        }

    }

    inline std::expected<OrderRequest, QString> buildOrderRequest(const QVariantMap& params)
    {
        OrderRequest req;

        // Обязательные
        req.m_category = stringToMarketType(detail::reqString(params, "category"));
        req.m_symbol = detail::reqString(params, "symbol");
        req.m_side = stringToOrderSide(detail::reqString(params, "side"));
        req.m_orderType = stringToOrderType(detail::reqString(params, "orderType"));
        req.m_qty = detail::reqString(params, "qty");

        // Опциональные
        req.m_price = detail::optString(params, "price");
        req.m_orderLinkId = detail::optString(params, "orderLinkId");
        req.m_takeProfit = detail::optString(params, "takeProfit");
        req.m_stopLoss = detail::optString(params, "stopLoss");
        req.m_triggerPrice = detail::optString(params, "triggerPrice");
        req.m_reduceOnly = detail::optBool(params, "reduceOnly");
        if (params.contains("timeInForce"))
            req.m_timeInForce = stringToTimeInForce(detail::reqString(params, "timeInForce"));

        // Обработка
        if (req.m_category == MarketType::Unknown)
            return std::unexpected<QString>("createOrder: Invalid \'category\' parameter");
        if (req.m_symbol.isEmpty())
            return std::unexpected<QString>("createOrder: The \'symbol\' parameter must not be empty");
        if (req.m_orderType == OrderType::Unknown)
            return std::unexpected<QString>("createOrder: Invalid \'orderType\' parameter");
        if (req.m_side == OrderSide::Unknown)
            return std::unexpected<QString>("createOrder: Invalid \'side\' parameter");
        if (req.m_qty.isEmpty())
            return std::unexpected<QString>("createOrder: The \'qty\' parameter must not be empty");
        if (req.m_orderType == OrderType::Limit && (!req.m_price.has_value() || req.m_price->isEmpty()))
            return std::unexpected<QString>("createOrder: The \'price\' parameter must not be empty for a limit order");

        return req;
    }

    inline std::expected<OrderAmendRequest, QString> buildOrderAmendRequest(const QVariantMap& params)
    {
        OrderAmendRequest req;

        // Обязательные
        req.m_category = stringToMarketType(detail::reqString(params, "category"));
        req.m_symbol = detail::reqString(params, "symbol");
        req.m_orderId = detail::optString(params, "orderId");
        req.m_orderLinkId = detail::optString(params, "orderLinkId");

        // Опциональные
        req.m_qty = detail::optString(params, "qty");
        req.m_price = detail::optString(params, "price");
        req.m_takeProfit = detail::optString(params, "takeProfit");
        req.m_stopLoss = detail::optString(params, "stopLoss");
        req.m_triggerPrice = detail::optString(params, "triggerPrice");

        // Обработка
        if (req.m_category == MarketType::Unknown)
            return std::unexpected<QString>("amendOrder: Invalid \'category\' parameter");
        if (req.m_symbol.isEmpty())
            return std::unexpected<QString>("amendOrder: The \'symbol\' parameter must not be empty");
        if (!detail::hasEitherId(req.m_orderId, req.m_orderLinkId))
            return std::unexpected<QString>("amendOrder: One of the parameters 'orderId' or 'orderLinkId' must be specified");

        return req;
    }

    inline std::expected<OrderCancelRequest, QString> buildOrderCancelRequest(const QVariantMap& params)
    {
        OrderCancelRequest req;

        // Обязательные
        req.m_category = stringToMarketType(detail::reqString(params, "category"));
        req.m_symbol = detail::reqString(params, "symbol");
        req.m_orderId = detail::optString(params, "orderId");
        req.m_orderLinkId = detail::optString(params, "orderLinkId");

        // Обработка
        if (req.m_category == MarketType::Unknown)
            return std::unexpected<QString>("cancelOrder: Invalid \'category\' parameter");
        if (req.m_symbol.isEmpty())
            return std::unexpected<QString>("cancelOrder: The \'symbol\' parameter must not be empty");
        if (!detail::hasEitherId(req.m_orderId, req.m_orderLinkId))
            return std::unexpected<QString>("cancelOrder: One of the parameters 'orderId' or 'orderLinkId' must be specified");

        return req;
    }

    inline std::expected<OrderCancelAllRequest, QString> buildOrderCancelAllRequest(const QVariantMap& params)
    {
        OrderCancelAllRequest req;

        // Обязательные
        req.m_category = stringToMarketType(detail::reqString(params, "category"));

        // Опциональные
        req.m_symbol = detail::optString(params, "symbol");
        req.m_baseCoin = detail::optString(params, "baseCoin");
        req.m_settleCoin = detail::optString(params, "settleCoin");

        const bool isDerivative = (req.m_category == MarketType::Linear || req.m_category == MarketType::Inverse);
        const bool hasFilter = (req.m_symbol.has_value() && !req.m_symbol->isEmpty()) ||
                               (req.m_baseCoin.has_value() && !req.m_baseCoin->isEmpty()) ||
                               (req.m_settleCoin.has_value() && !req.m_settleCoin->isEmpty());

        // Обработка
        if (isDerivative && !hasFilter)
            return std::unexpected<QString>("cancelAllOrders: For linear/inverse, a \'symbol\', \'baseCoin\', or \'settleCoin\' is required");

        return req;
    }

    inline std::expected<OpenOrdersRequest, QString> buildOpenOrdersRequest(const QVariantMap& params)
    {
        OpenOrdersRequest req;

        // Обязательные
        req.m_category = stringToMarketType(detail::reqString(params, "category"));

        req.m_symbol = detail::reqString(params, "symbol");
        req.m_orderId = detail::optString(params, "orderId");
        req.m_orderLinkId = detail::optString(params, "orderLinkId");
        req.m_openOnly = detail::optBool(params, "openOnly");

        // Обработка
        if (req.m_category == MarketType::Unknown)
            return std::unexpected<QString>("requestOpenOrders: Invalid \'category\' parameter");

        return req;
    }

    inline std::expected<OrderHistoryRequest, QString> buildOrderHistoryRequest(const QVariantMap& params)
    {
        OrderHistoryRequest req;

        // Обязательные
        req.m_category = stringToMarketType(detail::reqString(params, "category"));

        req.m_symbol = detail::reqString(params, "symbol");
        req.m_orderId = detail::optString(params, "orderId");
        req.m_orderLinkId = detail::optString(params, "orderLinkId");
        req.m_startTime = detail::optInt64(params, "startTime");
        req.m_endTime = detail::optInt64(params, "endTime");
        req.m_limit = detail::optInt64(params, "limit");

        // Обработка
        if (req.m_category == MarketType::Unknown)
            return std::unexpected<QString>("requestOrderHistory: Invalid \'category\' parameter");
        if (req.m_startTime.has_value() &&
            req.m_endTime.has_value() &&
            *req.m_startTime > *req.m_endTime)
            return std::unexpected("requestOrderHistory: \'startTime\' cannot be later than \'endTime\'");

        return req;
    }

}