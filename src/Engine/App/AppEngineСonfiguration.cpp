#include "Engine/App/AppEngineСonfiguration.hpp"
#include "Engine/Managers/JsonManager.hpp"

#include <QFileInfo>
#include <QDir>

// Файл конфигурации
const char* configFile = "platform.json";
// Главный объект конфигурации
const char* configObject = "Platform";

AppEngineСonfiguration::AppEngineСonfiguration() {

}

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

bool AppEngineСonfiguration::openСonfigurationFile() {
    // Получаем домашнюю директорию
    QString homePath = QDir::homePath();

    // Формируем полный путь
    QString configDir = homePath + "/.config/ByBit";
    m_fullNameConfigFile = QString("%1/%2").arg(configDir).arg(configFile);

    // Создаем директорию .config если не существует
    QDir dir;
    if (!dir.exists(configDir)) {
        if (!dir.mkpath(configDir)) {
            m_lastError = "Failed to create directory:" + configDir;
            return false;
        }
    }

    // Проверяем наличие конфигурации
    QFileInfo checkFile(m_fullNameConfigFile);
    if (checkFile.isFile() && !checkFile.exists())
        if (!writeСonfiguration())
            return false;

    return true;
}

bool AppEngineСonfiguration::readСonfiguration() {
    JsonManager jsManager;

    auto doc = jsManager.readDocument(m_fullNameConfigFile);
    if (!doc.has_value()) {
        m_lastError = doc.error();
        return false;
    }

    parseJsonDocument(doc.value());
    return true;
}

bool AppEngineСonfiguration::writeСonfiguration() {
    QJsonObject root;
    root[configObject] = AppEngineСonfiguration::instance().toJson();
    QJsonDocument doc(root);

    JsonManager jsManager;
    jsManager.setDocument(doc);
    auto isError = jsManager.writeDocument(m_fullNameConfigFile);
    if (isError.has_value()) {
        m_lastError = isError.value();
        return false;
    }

    return true;

}