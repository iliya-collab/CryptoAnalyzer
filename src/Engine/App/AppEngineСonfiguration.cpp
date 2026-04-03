#include "Engine/App/AppEngineСonfiguration.hpp"
#include "Engine/Managers/JsonManager.hpp"

#include <QDebug>

const char* configFile = "platform.json";
const char* configObject = "Platform";

QJsonObject AppEngineСonfiguration::toJson() {
    QJsonObject platform;

    QJsonObject objKeys;
    for (auto [name, key] : m_config.m_api.asKeyValueRange()) {
        QJsonObject objAPIKey;
        objAPIKey["api_key"] = key.api_key;
        objAPIKey["secret_key"] = key.secret_key;
        objAPIKey["testnet"] = key.testnet;
        objKeys[name] = objAPIKey;
    }
    platform["Keys"] = objKeys;

    return platform;
}

void AppEngineСonfiguration::fromJson(const QJsonObject& obj) {
    QJsonObject objKeys = obj.value("Keys").toObject();

    m_config.m_api.clear();
    for (auto it = objKeys.begin(); it != objKeys.end(); ++it) {
        QString name = it.key();
        
        QJsonObject objAPIKey = it.value().toObject();

        QString api_key = objAPIKey.value("api_key").toString();
        QString secret_key = objAPIKey.value("secret_key").toString();
        bool testnet = objAPIKey.value("testnet").toBool();

        m_config.m_api[name] = {
            api_key, 
            secret_key,
            testnet
        };
    }
}

void AppEngineСonfiguration::parseJsonDocument(const QJsonDocument& doc) {
    QJsonObject root = doc.object();
    AppEngineСonfiguration::instance().fromJson(root.value(configObject).toObject());
}

bool AppEngineСonfiguration::readСonfiguration() {
    QJsonDocument doc;

    auto exp = JsonManager::readDocument(configFile);
    if (!exp.has_value()) {
        m_lastError = exp.error();
        return false;
    }

    doc = exp.value();

    exp = JsonManager::isDocumentValid(doc);
    if (!exp.has_value()) {
        m_lastError = exp.error();
        return false;
    }

    parseJsonDocument(doc);

    return true;
}

void AppEngineСonfiguration::writeСonfiguration() {
    QJsonObject root;
    root[configObject] = AppEngineСonfiguration::instance().toJson();
    QJsonDocument doc(root);
    JsonManager::setDocument(doc);
    JsonManager::writeDocument(configFile);
}