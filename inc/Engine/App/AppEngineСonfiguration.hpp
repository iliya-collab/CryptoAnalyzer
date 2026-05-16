#pragma once

#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QHash>

#include "Engine/StdTypes.hpp"

namespace Engine {

    class AppEngineСonfiguration : public QObject {
    private:

        // Структура параметров конфигурации
        struct ParamsСonfiguration {
            // Набор API ключей
            QHash<QString, API> m_api;
            // Ключ по умолчанию
            QString m_defaultKey;
        };

        // Файл конфигурации
        static const QString configFile;
        // Главный объект конфигурации
        static const char* configObject;

        // Параметры конфигурации
        ParamsСonfiguration m_config;
        // Последняя ошибка
        QString m_lastError;

        QJsonObject toJson();
        void fromJson(const QJsonObject& obj);

        void parseJsonDocument(const QJsonDocument& doc);

        bool readСonfiguration();
        bool writeСonfiguration();

    public:

        AppEngineСonfiguration();
        ~AppEngineСonfiguration();

        void loadConfig();
        void saveARIKey(const QString& name, API api);

        // Получает текущий конфиг
        ParamsСonfiguration& getСonfiguration() {
            return m_config;
        }

        QString getLastError() {
            return m_lastError;
        }

    };

}