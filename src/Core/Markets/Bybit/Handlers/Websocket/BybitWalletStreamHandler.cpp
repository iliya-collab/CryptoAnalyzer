#include "BybitWalletStreamHandler.hpp"

void Core::Markets::BybitWalletStreamHandler::handle(const QJsonObject &data, IPrivateStreamer *streamer)
{
    if (!data.contains("data") || !data["data"].isArray())
    {
        emit streamer->errorOccurred(streamer->id(), "[" + topic() + "] Invalid response structure!");
        return;
    }

    QJsonArray dataArray = data["data"].toArray();
    if (dataArray.isEmpty())
        return;

    QJsonObject dataObj = dataArray.at(0).toObject();

    Tools::AccountBalance balance{};

    balance.m_totalWalletBalance = dataObj["totalWalletBalance"].toString().toDouble();

    QJsonArray coins = dataObj["coin"].toArray();

    balance.m_assets.reserve(coins.size());

    for (const auto& coin : std::as_const(coins))
    {
        QJsonObject objCoin = coin.toObject();

        QString nameAsset = objCoin["coin"].toString();
        double sizeAsset = objCoin["walletBalance"].toString().toDouble();
        double priceAsset = objCoin["usdValue"].toString().toDouble();

        balance.m_assets.emplace_back(nameAsset, sizeAsset, priceAsset);
    }

    emit streamer->walletUpdated(balance);
}
