#include "ConfigurationManager.hpp"
#include <QFile>
#include <QFileInfo>
#include <QJsonObject>
#include <QJsonDocument>

namespace Core {

    ConfigurationManager::ConfigurationManager() {
        if (!QFile::exists(m_configFile))
            generateDefaultConfiguration();
    }

    void ConfigurationManager::generateDefaultConfiguration() {
        QFile file(m_configFile);

        if (!file.open(QIODevice::WriteOnly))
            return;

        // Корневой элемент конфигурации
        QJsonObject root;

        // Конфигурация API
        QJsonObject apiObj;
        apiObj["Key"] = "";
        apiObj["SecretKey"] = "";
        apiObj["isTestnet"] = false;
        root["API"] = apiObj;

        // Конфигурация General
        QJsonObject generalObj;
        generalObj["AutoConnection"] = true;
        root["General"] = generalObj;

        QJsonDocument doc(root);
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
    }

    std::expected<ConfigurationManager::ConfigurationParams, QString> ConfigurationManager::read() {
        QFile file(m_configFile);

        if (!file.open(QIODevice::ReadOnly))
            return std::unexpected("Error opening configuration file!");

        QByteArray fileData = file.readAll();

        file.close();

        QJsonDocument doc = QJsonDocument::fromJson(fileData);
        QJsonObject root = doc.object();

        QJsonObject apiObj = root["API"].toObject();
        m_paramsConfig.m_apiKey = apiObj["Key"].toString();
        m_paramsConfig.m_secretKey = apiObj["SecretKey"].toString();
        m_paramsConfig.m_isTestnet = apiObj["isTestnet"].toBool();

        QJsonObject generalObj = root["General"].toObject();
        m_paramsConfig.m_autoConnection = generalObj["AutoConnection"].toBool();

        return m_paramsConfig;
    }

    void ConfigurationManager::write() {
        QFile file(m_configFile);

        if (!file.open(QIODevice::WriteOnly))
            return;

        // Корневой элемент конфигурации
        QJsonObject root;

        // Конфигурация API
        QJsonObject apiObj;
        apiObj["Key"] = m_paramsConfig.m_apiKey;
        apiObj["SecretKey"] = m_paramsConfig.m_secretKey;
        apiObj["isTestnet"] = m_paramsConfig.m_isTestnet;
        root["API"] = apiObj;

        // Конфигурация General
        QJsonObject generalObj;
        generalObj["AutoConnection"] = m_paramsConfig.m_autoConnection;
        root["General"] = generalObj;


        QJsonDocument doc(root);
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
    }

    void ConfigurationManager::setAPI(const QString& apiKey, const QString& secretKey, bool isTestnet) {
        m_paramsConfig.m_apiKey = apiKey;
        m_paramsConfig.m_secretKey = secretKey;
        m_paramsConfig.m_isTestnet = isTestnet;
    }

}