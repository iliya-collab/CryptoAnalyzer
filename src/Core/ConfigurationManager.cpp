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
        QJsonObject apisObj;
        QJsonObject defaultApi;
        defaultApi["Key"] = "";
        defaultApi["SecretKey"] = "";
        defaultApi["isTestnet"] = false;
        apisObj["default"] = defaultApi;
        root["APIs"] = apisObj;

        // Конфигурация General
        QJsonObject generalObj;
        generalObj["AutoConnection"] = true;
        generalObj["ActiveAPI"] = "default";
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

        QJsonObject apisObj = root["APIs"].toObject();
        for (auto it = apisObj.begin(); it != apisObj.end(); ++it) {
            QString name = it.key();
            QJsonObject apiObj = it.value().toObject();
            Tools::API api;
            api.m_apiKey = apiObj["Key"].toString();
            api.m_secretKey = apiObj["SecretKey"].toString();
            api.m_isTestnet = apiObj["isTestnet"].toBool();
            m_paramsConfig.m_ApiSet[name] = api;
        }

        QJsonObject generalObj = root["General"].toObject();
        m_paramsConfig.m_autoConnection = generalObj["AutoConnection"].toBool();
        m_paramsConfig.m_activeAPI = generalObj["ActiveAPI"].toString();

        return m_paramsConfig;
    }

    void ConfigurationManager::write() {
        QFile file(m_configFile);

        if (!file.open(QIODevice::WriteOnly))
            return;

        // Корневой элемент конфигурации
        QJsonObject root;

        // Конфигурация API
        QJsonObject apisObj;
        for (auto it = m_paramsConfig.m_ApiSet.begin(); it != m_paramsConfig.m_ApiSet.end(); ++it) {
            QJsonObject apiObj;
            Tools::API api = it.value();
            apiObj["Key"] = api.m_apiKey;
            apiObj["SecretKey"] = api.m_secretKey;
            apiObj["isTestnet"] = api.m_isTestnet;
            apisObj[it.key()] = apiObj;
        }
        root["APIs"] = apisObj;

        // Конфигурация General
        QJsonObject generalObj;
        generalObj["AutoConnection"] = m_paramsConfig.m_autoConnection;
        generalObj["ActiveAPI"] = m_paramsConfig.m_activeAPI;
        root["General"] = generalObj;

        QJsonDocument doc(root);
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
    }

    void ConfigurationManager::addAPI(const QString& name, const Tools::API& api)
    {
        if (!m_paramsConfig.m_ApiSet.contains(name))
            m_paramsConfig.m_ApiSet[name] = api;
    }

    void ConfigurationManager::removeAPI(const QString &name)
    {
        if (name != m_paramsConfig.m_activeAPI)
            m_paramsConfig.m_ApiSet.remove(name);
    }

}