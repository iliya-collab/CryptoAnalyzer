#include "Configs/PlatformConfig.hpp"

#include <QDebug>

QJsonObject PlatformConfig::toJson() {
    QJsonObject scanner;

    QJsonArray arrPairs;
    for (auto i : _config.pairs)
        arrPairs.append(i);
    scanner["Pairs"] = arrPairs;

    return scanner;
}

void PlatformConfig::setDefaultConfig() {
    _config.pairs = { "BTCUSDT" };
}

void PlatformConfig::fromJson(const QJsonObject& obj) {
    QJsonArray arrPairs = obj.value("Pairs").toArray();
    _config.pairs.clear();
    for (int i = 0; i < arrPairs.size(); i++)
        _config.pairs.append(arrPairs[i].toString());
}

std::expected<QJsonObject, QString> PlatformConfig::isJsonObjectValid(const QJsonObject& obj) {
    return obj;
}
