#pragma once

#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QHash>

class AppEngineСonfiguration {
private:

    struct API {
        QString api_key;
        QString secret_key;
        bool testnet;
    };

    struct ParamsСonfiguration {
        QHash<QString, API> m_api;
    };

    ParamsСonfiguration m_config;

    QString m_lastError;

    AppEngineСonfiguration() = default;

    QJsonObject toJson();
    void fromJson(const QJsonObject& obj);

    void parseJsonDocument(const QJsonDocument& doc);

public:

    ~AppEngineСonfiguration() = default;
    AppEngineСonfiguration(const AppEngineСonfiguration&) = delete;
    AppEngineСonfiguration& operator=(const AppEngineСonfiguration&) = delete;

    bool readСonfiguration();
    void writeСonfiguration();

    // Получает текущий конфиг
    ParamsСonfiguration& getСonfiguration() {
        return m_config;
    }

    // Возвращает экземпляр класса
    static AppEngineСonfiguration& instance() {
        static AppEngineСonfiguration instance;
        return instance;
    }

    QString getLastError() {
        return m_lastError;
    }

};
