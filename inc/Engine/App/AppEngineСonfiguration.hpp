#pragma once

#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QHash>

class AppEngineСonfiguration {
private:

    // Структура для хранения информации об API ключах
    struct API {
        // API ключ
        QString api_key;
        // Секретный API ключ
        QString secret_key;
        // Тип сети (true - testnet, false - mainnet)
        bool testnet;
    };

    // Структура параметров конфигурации
    struct ParamsСonfiguration {
        // Набор API ключей
        QHash<QString, API> m_api;
    };

    // Параметры конфигурации
    ParamsСonfiguration m_config;
    // Последняя ошибка
    QString m_lastError;
    QString m_fullNameConfigFile;

    AppEngineСonfiguration();

    QJsonObject toJson();
    void fromJson(const QJsonObject& obj);

    void parseJsonDocument(const QJsonDocument& doc);

public:

    ~AppEngineСonfiguration() = default;
    AppEngineСonfiguration(const AppEngineСonfiguration&) = delete;
    AppEngineСonfiguration& operator=(const AppEngineСonfiguration&) = delete;

    bool openСonfigurationFile();
    bool readСonfiguration();
    bool writeСonfiguration();

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
