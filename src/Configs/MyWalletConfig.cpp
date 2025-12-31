#include "Configs/MyWalletConfig.hpp"

QJsonObject MyWalletConfig::toJson() {

    QJsonObject assets;
    for (auto [name, amount] : _config.asset.asKeyValueRange())
        assets[name] = amount;

    QJsonObject wallet;
    wallet["Assets"] = assets;

    return wallet;
}

void MyWalletConfig::fromJson(const QJsonObject& obj) {

    QJsonObject asset = obj.value("Assets").toObject();
    _config.asset.clear();
    for (auto it = asset.begin(); it != asset.end(); ++it)
        _config.asset[it.key()] = it.value().toDouble();
}
