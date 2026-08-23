#pragma once
#include <QString>
#include <QUrlQuery>

template<typename T>
concept HasEndpoint = requires()
{
    { T::endpoint() } -> std::convertible_to<QString>;
};

template<typename T>
concept HasBalanceRequest = requires()
{
    { T::buildRequest() } -> std::convertible_to<QUrlQuery>;
};

template<typename T>
concept HasKlinesRequest = requires(const QString& category, const QString& symbol, const QString& interval, qint64 start, qint64 end)
{
    { T::buildRequest(category, symbol, interval, start, end) } -> std::convertible_to<QUrlQuery>;
};

template<typename T>
concept HasTradePairsRequest = requires(const QString& category)
{
    { T::buildRequest(category) } -> std::convertible_to<QUrlQuery>;
};