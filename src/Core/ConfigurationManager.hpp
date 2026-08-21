#pragma once
#include "Tools/StdTypes.hpp"
#include <QString>
#include <expected>

namespace Core {

    class ConfigurationManager {
    private:

        // Параметры конфигурации
        struct ConfigurationParams {
            // Общее
            bool m_autoConnection = true; // Автоматическое подключение приложения к бирже
            QString m_activeAPI = "";
            // Набор ключей
            QMap<QString, Tools::API> m_ApiSet{};
        };

    public:

        static ConfigurationManager& instance() {
            static ConfigurationManager config;
            return config;
        }

        // Метод для получения текущей конфигурации
        ConfigurationParams config() { return m_paramsConfig; }
        // Метод для чтения файла конфигурации
        std::expected<ConfigurationParams, QString> read();
        // Метод для записи текущей конфигурации
        void write();

// ==================================   Методы для задания параметров конфигурации  ==================================
        void addAPI(const QString& name, const Tools::API& api);
        void removeAPI(const QString& name);

    private:

        ConfigurationManager();
        ConfigurationManager(const ConfigurationManager&) = delete;
        ConfigurationManager& operator=(const ConfigurationManager&) = delete;

        // Создает конфигурацию по умолчания
        void generateDefaultConfiguration();
        // Текущая конфигурация
        ConfigurationParams m_paramsConfig;
        // Файл конфигурации
        const char* m_configFile = "configuration.json";

    };

}
