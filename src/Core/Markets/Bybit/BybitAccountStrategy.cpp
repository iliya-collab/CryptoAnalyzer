#include "BybitAccountStrategy.hpp"

namespace Core::Markets
{

    QString BybitAccountStrategy::targetEndpoint() const { return "/v5/account/info"; }

    void BybitAccountStrategy::handle(const QJsonObject &data, IContext *context)
    {
        emit context->infoAboutAccountReceived(data["retMsg"] == "OK");
    }


}