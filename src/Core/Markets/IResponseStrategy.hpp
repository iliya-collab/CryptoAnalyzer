#pragma once
#include "IContext.hpp"

namespace Core::Markets
{
    class IResponseStrategy
    {
    public:
        virtual ~IResponseStrategy() = default;
        // Метод, который принимает сырой JSON и выполняет парсинг
        virtual void handle(const QJsonObject& data, class IContext* context) = 0;
        // Возвращает эндпоинт, за который отвечает эта стратегия
        virtual QString targetEndpoint() const = 0;
    };
}
