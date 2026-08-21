#include "BybitTradePairsStrategy.hpp"

namespace Core::Markets
{

    void BybitTradePairsStrategy::processRequestTradePairs(QList<Tools::TradeInfo> &pairs, const QJsonObject &data)
    {
        QJsonObject result = data["result"].toObject();
        QString category = result["category"].toString();
        QJsonArray list = result["list"].toArray();

        for (const auto& obj : list)
        {
            QJsonObject item = obj.toObject();
            Tools::TradeInfo info;
            info.symbol = item["symbol"].toString();
            info.base_coin = item["baseCoin"].toString();
            info.quote_coin = item["quoteCoin"].toString();
            pairs.append(info);
        }
    }

    QString BybitTradePairsStrategy::targetEndpoint() const { return "/v5/market/instruments-info"; }

    void BybitTradePairsStrategy::handle(const QJsonObject &data, IContext *context)
    {
        QList<Tools::TradeInfo> pairs;
        processRequestTradePairs(pairs, data);
        emit context->tradePairsReceived(pairs);
    }

}
