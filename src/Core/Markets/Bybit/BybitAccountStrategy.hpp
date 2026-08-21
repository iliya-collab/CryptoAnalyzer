#pragma once
#include "Markets/IResponseStrategy.hpp"

namespace Core::Markets
{
\
    class BybitAccountStrategy : public IResponseStrategy
    {
    public:

        QString targetEndpoint() const override;

        void handle(const QJsonObject& data, IContext* context) override;
    };

}

