#pragma once
#include <QString>
#include <QByteArray>
#include <QUrlQuery>
#include <concepts>

namespace Core::Markets
{

    // Концепт для проверки endpoint
    template<typename T>
    concept HasEndpoint = requires()
    {
        { T::endpoint() } -> std::convertible_to<QString>;
    };

    // Концепт для построения параметров в url
    template<typename T, typename... Args>
    concept HasBuildQuery = requires(Args&&... args)
    {
        { T::buildRequest(std::forward<Args>(args)...) } -> std::convertible_to<QUrlQuery>;
    };

    // Концепт для построения параметров в теле запроса
    template<typename T, typename... Args>
    concept HasBuildBody = requires(Args&&... args) {
        { T::buildRequestBody(std::forward<Args>(args)...) } -> std::convertible_to<QByteArray>;
    };

    template<typename T, typename... Args>
    concept IsGetRequest = HasEndpoint<T> && HasBuildQuery<T, Args...> && !HasBuildBody<T, Args...>;

    template<typename T, typename... Args>
    concept IsPostRequest = HasEndpoint<T> && HasBuildBody<T, Args...> && !HasBuildQuery<T, Args...>;

}


