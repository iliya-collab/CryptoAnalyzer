#pragma once
#include "IMarketDataService.hpp"
#include "IOrderService.hpp"

namespace Core::Markets
{

template <typename TService>
class IResponseHandler
{
public:
    virtual ~IResponseHandler() = default;
    // Метод, который принимает сырой JSON и выполняет парсинг
    virtual void handle(const QJsonObject& data, TService* service) = 0;
};

using IMarketDataResponseHandler = IResponseHandler<IMarketDataService>;
using IOrderResponseHandler = IResponseHandler<IOrderService>;

}
