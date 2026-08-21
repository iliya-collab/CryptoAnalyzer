#pragma once
#include "Markets/IResponseStrategy.hpp"

namespace Core::Markets
{
    class BybitKlineStrategy : public IResponseStrategy
    {
    private:

        void processRequestKlines(QList<Tools::Kline>& klines, const QString& interval, const QJsonObject& data);

    public:

        QString targetEndpoint() const override;

        void handle(const QJsonObject& data, IContext* context) override;

    };
}