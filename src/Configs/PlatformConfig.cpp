#include "Configs/PlatformConfig.hpp"

#include <QDebug>

QJsonObject PlatformConfig::toJson() {
    QJsonObject platform;

    QJsonObject objKeys;
    for (auto [name, key] : _config.keys.asKeyValueRange()) {
        QJsonObject objAPIKey;
        objAPIKey["api_key"] = key.api_key;
        objAPIKey["secret_key"] = key.secret_key;
        objAPIKey["testnet"] = key.testnet;
        objKeys[name] = objAPIKey;
    }
    platform["Keys"] = objKeys;

    return platform;
}

void PlatformConfig::fromJson(const QJsonObject& obj) {
    QJsonObject objKeys = obj.value("Keys").toObject();

    _config.keys.clear();
    for (auto it = objKeys.begin(); it != objKeys.end(); ++it) {
        QString name = it.key();
        
        QJsonObject objAPIKey = it.value().toObject();

        QString api_key = objAPIKey.value("api_key").toString();
        QString secret_key = objAPIKey.value("secret_key").toString();
        bool testnet = objAPIKey.value("testnet").toBool();

        _config.keys[name] = {
            api_key, 
            secret_key,
            testnet
        };

        qDebug().noquote() << QString("Key found\n\tName : '%1'\n\tapi_key : %2\n\tsecret_key : %3\n\ttestnet : %4")
            .arg(name)
            .arg(api_key)
            .arg(secret_key)
            .arg(testnet);
    }
}