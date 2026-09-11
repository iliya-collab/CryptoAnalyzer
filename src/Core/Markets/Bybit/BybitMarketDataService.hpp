#pragma once
#include "Markets/RestAPI/BaseMarketDataService.hpp"

namespace Core::Markets
{

class BybitMarketDataService : public BaseMarketDataService
{
    Q_OBJECT
public:

    explicit BybitMarketDataService(QObject* parent = nullptr);

    void init(bool isTestnet) override;

    void requestTradePairs(Tools::MarketType type) override;
    void requestKlines(Tools::MarketType type, const QString& symbol, const QString& interval, qint64 start, qint64 end) override;

};

}

