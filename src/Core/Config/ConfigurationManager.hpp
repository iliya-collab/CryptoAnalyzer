#pragma once
#include "Configuration.hpp"
#include <expected>

namespace Core {

class ConfigurationManager
{
public:

    static ConfigurationManager& instance();

    static QString configPath();

    // Метод для получения текущей конфигурации
    const ConfigurationParams& data() const noexcept { return m_data; }
    // Метод для чтения файла конфигурации
    std::expected<ConfigurationParams, QString> load();
    // Метод для записи текущей конфигурации
    std::expected<void, QString> save();

// ==================================   Методы для задания параметров конфигурации  ==================================
    std::expected<void, QString>  addApi(const QString& name, const Tools::Api& api);
    std::expected<void, QString>  removeApi(const QString& name);
    void setAutoConnection(bool enabled);
    std::expected<void, QString> setActiveApi(const QString& name);

private:

    ConfigurationManager();
    ConfigurationManager(const ConfigurationManager&) = delete;
    ConfigurationManager& operator=(const ConfigurationManager&) = delete;

    // Создает конфигурацию по умолчания
    void generateDefaultConfiguration();
    // Текущая конфигурация
    ConfigurationParams m_data;

};

}
