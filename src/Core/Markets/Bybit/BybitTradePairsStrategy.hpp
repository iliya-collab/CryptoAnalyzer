#pragma once
#include "Markets/IResponseStrategy.hpp"

namespace Core::Markets
{

    class BybitTradePairsStrategy : public IResponseStrategy
    {
    private:

        void processRequestTradePairs(QList<Tools::TradeInfo>& pairs, const QJsonObject& data);

    public:

        QString targetEndpoint() const override;

        void handle(const QJsonObject& data, IContext* context) override;
    };

}

