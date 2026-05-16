#include "Engine/App/AppEngineСonfiguration.hpp"
#include "Engine/Managers/JsonManager.hpp"

#include <QFileInfo>
#include <QDir>
#include <QThread>

namespace Engine {

    const QString AppEngineСonfiguration::configFile = QDir::homePath() + "/.config/ByBit/platform.json";
    const char* AppEngineСonfiguration::configObject = "Platform";

    AppEngineСonfiguration::AppEngineСonfiguration() {
        qDebug() << Q_FUNC_INFO << "created in:" << QThread::currentThread();
    }

    AppEngineСonfiguration::~AppEngineСonfiguration() {
        qDebug() << Q_FUNC_INFO << "launched from:" << QThread::currentThread();
        qDebug() << Q_FUNC_INFO << "finished";
    }

    QJsonObject AppEngineСonfiguration::toJson() {
        QJsonObject platform;

        QJsonObject objKeys;
        for (auto [name, key] : m_config.m_api.asKeyValueRange()) {
            QJsonObject objAPIKey;
            objAPIKey["apiKey"] = key.m_apiKey;
            objAPIKey["secretKey"] = key.m_secretKey;
            objAPIKey["isTestnet"] = key.m_isTestnet;
            objKeys[name] = objAPIKey;
        }

        platform["usedKeys"] = objKeys;
        platform["defaultKey"] = m_config.m_defaultKey;

        return platform;
    }

    void AppEngineСonfiguration::fromJson(const QJsonObject& obj) {
        m_config.m_defaultKey = obj.value("defaultKey").toString();

        QJsonObject objKeys = obj.value("usedKeys").toObject();

        m_config.m_api.clear();
        for (auto it = objKeys.begin(); it != objKeys.end(); ++it) {
            QString name = it.key();

            QJsonObject objAPIKey = it.value().toObject();

            QString api_key = objAPIKey.value("apiKey").toString();
            QString secret_key = objAPIKey.value("secretKey").toString();
            bool testnet = objAPIKey.value("isTestnet").toBool();

            m_config.m_api[name] = {
                api_key,
                secret_key,
                testnet
            };
        }
    }

    void AppEngineСonfiguration::parseJsonDocument(const QJsonDocument& doc) {
        QJsonObject root = doc.object();
        fromJson(root.value(configObject).toObject());
    }

    bool AppEngineСonfiguration::readСonfiguration() {
        JsonManager jsManager;

        auto doc = jsManager.readDocument(configFile);
        if (!doc.has_value()) {
            m_lastError = doc.error();
            return false;
        }

        parseJsonDocument(doc.value());
        return true;
    }

    bool AppEngineСonfiguration::writeСonfiguration() {
        QJsonObject root;
        root[configObject] = toJson();
        QJsonDocument doc(root);

        JsonManager jsManager;
        jsManager.setDocument(doc);
        auto isError = jsManager.writeDocument(configFile);
        if (isError.has_value()) {
            m_lastError = isError.value();
            return false;
        }

        return true;
    }

    void AppEngineСonfiguration::loadConfig() {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (!readСonfiguration()) {
            qDebug().noquote() << "The configuration was not read:" << m_lastError;
            return;
        }

        if (m_config.m_api.isEmpty()) {
            qDebug().noquote() << "No API configuration found!";
            return;
        }

        for (auto [name, key] : m_config.m_api.asKeyValueRange()) {
            if (key.m_apiKey.isEmpty())
                qDebug().noquote() << QString("In key \'%1\': API key is empty").arg(name);
            else if (key.m_secretKey.isEmpty())
                qDebug().noquote() << QString("In key \'%1\': Secret key is empty").arg(name);
            else
                qDebug().noquote() << QString(" * Key \'%1\' detected").arg(name);
        };

        if (m_config.m_defaultKey.isEmpty())
            m_config.m_defaultKey = m_config.m_api.keys().value(0);

        qDebug().noquote() << " * Default key:" << QString("\'%1\'").arg(m_config.m_defaultKey);
    }

    void AppEngineСonfiguration::saveARIKey(const QString& name, API api) {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (!readСonfiguration()) {
            qDebug().noquote() << "The configuration was not read:" << m_lastError;
            return;
        }

        m_config.m_api.insert(name, api);

        if (!writeСonfiguration()) {
            qDebug().noquote() << "The configuration was not write:" << m_lastError;
            return;
        }
    }

}