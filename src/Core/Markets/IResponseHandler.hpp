#pragma once
#include "IMarketService.hpp"

namespace Core::Markets
{
    class IResponseHandler
    {
    public:
        virtual ~IResponseHandler() = default;
        // Метод, который принимает сырой JSON и выполняет парсинг
        virtual void handle(const QJsonObject& data, class IMarketService* service) = 0;
    };
}
