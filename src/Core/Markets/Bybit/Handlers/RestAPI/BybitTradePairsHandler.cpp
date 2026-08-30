#include "BybitTradePairsHandler.hpp"

namespace Core::Markets
{

    void BybitTradePairsHandler::processRequestTradePairs(QList<Tools::TradeInfo> &pairs, const QJsonObject &data)
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

    void BybitTradePairsHandler::handle(const QJsonObject &data, IMarketService *service)
    {
        if (!data.contains("retMsg") || data["retMsg"].toString() != "OK")
        {
            emit service->errorOccurred(QString("Error processing request [endpoint = %1]: " + data["retMsg"].toString()).arg(endpoint()));
            return;
        }

        QList<Tools::TradeInfo> pairs;
        processRequestTradePairs(pairs, data);
        emit service->tradePairsReceived(pairs);
    }

}
