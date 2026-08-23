#include "BybitAccountBalanceHandler.hpp"

namespace Core::Markets
{

    void BybitAccountBalanceHandler::handle(const QJsonObject &data, IMarketService* service)
    {
        emit service->accountBalanceReceived(data["retMsg"] == "OK");
    }


}