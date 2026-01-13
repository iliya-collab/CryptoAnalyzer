#include "Configs/ScannerConfig.hpp"

QJsonObject ScannerConfig::toJson() {
    QJsonObject scanner;

    QJsonArray arrPairs;
    for (auto i : _config.pairs)
        arrPairs.append(i);
    scanner["Pairs"] = arrPairs;

    return scanner;
}

void ScannerConfig::setDefaultConfig() {
    _config.pairs = { "BTCUSDT" };
}

void ScannerConfig::fromJson(const QJsonObject& obj) {
    QJsonArray arrPairs = obj.value("Pairs").toArray();
    _config.pairs.clear();
    for (int i = 0; i < arrPairs.size(); i++)
        _config.pairs.append(arrPairs[i].toString());
}
