#include "Configs/MyWalletConfig.hpp"

QJsonObject MyWalletConfig::toJson() {

    QJsonObject assets;
    for (auto [name, amount] : _config.asset.asKeyValueRange())
        assets[name] = amount;

    QJsonObject rules;
    for (auto [name, align] : _config.rules.asKeyValueRange())
        rules[name] = align;

    QJsonObject wallet;
    wallet["Assets"] = assets;
    wallet["Rules"] = rules;

    return wallet;
}

void MyWalletConfig::fromJson(const QJsonObject& obj) {

    QJsonObject asset = obj.value("Assets").toObject();
    _config.asset.clear();
    for (auto it = asset.begin(); it != asset.end(); ++it)
        _config.asset[it.key()] = it.value().toDouble();

    QJsonObject rules = obj.value("Rules").toObject();
    _config.rules.clear();
    for (auto it = rules.begin(); it != rules.end(); ++it)
        _config.rules[it.key()] = it.value().toString();
}
