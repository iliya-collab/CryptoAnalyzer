#include "BybitAccountBalanceHandler.hpp"

namespace Core::Markets
{

    void BybitAccountBalanceHandler::handle(const QJsonObject &data, IMarketService* service)
    {
        //qDebug() << QJsonDocument(data).toJson(QJsonDocument::Compact);

        if (!data.contains("retMsg") || data["retMsg"].toString() != "OK")
        {
            emit service->errorOccurred(QString("Error processing request [endpoint = %1]: " + data["retMsg"].toString()).arg(endpoint()));
            return;
        }

        emit service->accountVerified();

        Tools::AccountBalance balance{};

        QJsonObject result = data["result"].toObject();
        QJsonArray list = result["list"].toArray();

        if (list.isEmpty())
            return;

        QJsonObject obj = list[0].toObject();

        balance.m_totalWalletBalance = obj["totalWalletBalance"].toString().toDouble();

        QJsonArray coins = obj["coin"].toArray();

        balance.m_assets.reserve(coins.size());

        for (const auto& coin : coins)
        {
            QJsonObject objCoin = coin.toObject();

            QString nameAsset = objCoin["coin"].toString();
            double sizeAsset = objCoin["walletBalance"].toString().toDouble();
            double priceAsset = objCoin["usdValue"].toString().toDouble();

            balance.m_assets.emplace_back(nameAsset, sizeAsset, priceAsset);
        }

        // for (const auto& [name, amount, value] : balance.m_assets)
        //     qDebug() << name << amount << value;

        emit service->accountBalanceReceived(balance);
    }


}