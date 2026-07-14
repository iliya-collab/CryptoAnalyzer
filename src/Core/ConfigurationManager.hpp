#pragma once
#include <QString>
#include <expected>

namespace Core {

    class ConfigurationManager {
    private:

        // Параметры конфигурации
        struct ConfigurationParams {
            // Общее
            bool m_autoConnection = true; // Автоматическое подключение приложения к бирже
            // Настройка API
            QString m_apiKey = ""; // API ключ
            QString m_secretKey = ""; // Секретный API ключ
            bool m_isTestnet = false; // Тип сети (true - testnet, false - mainnet)
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
        void setAPI(const QString& apiKey, const QString& secretKey, bool isTestnet);

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
