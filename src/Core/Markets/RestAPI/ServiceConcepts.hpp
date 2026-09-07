#pragma once
#include <QString>
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

    // Универсальный концепт для buildRequest с любым числом и типом параметров
    template<typename T, typename... Args>
    concept HasBuildRequest = requires(Args&&... args)
    {
        { T::buildRequest(std::forward<Args>(args)...) } -> std::convertible_to<QUrlQuery>;
    };

}


