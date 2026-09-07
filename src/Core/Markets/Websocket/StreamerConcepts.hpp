#pragma once
#include <QString>
#include <concepts>

namespace Core::Markets
{

// Концепт для проверки topic
template<typename T>
concept HasTopic = requires()
{
    { T::topic() } -> std::convertible_to<QString>;
};

}


