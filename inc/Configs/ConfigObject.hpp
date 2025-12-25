#ifndef CONFIG_OBJECT_HPP
#define CONFIG_OBJECT_HPP

/*
ConfigObject - кдасс для работы с config
Тип TDerived - класс, который работает с данным config
Тип TConfig - структура, описывающая сам config
*/

#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

template <typename TDerived, typename TConfig>
class ConfigObject {

protected:

    TConfig _config;

    ConfigObject() = default;

public:

    virtual ~ConfigObject() = default;
    ConfigObject(const ConfigObject&) = delete;
    
    ConfigObject& operator=(const ConfigObject&) = delete;

    // Формирует из текущего конфига QJsonObject и возвращает его в качестве значения
    virtual QJsonObject toJson() = 0;

    // Читает полученный QJsonObject объект и распределяет его поля в свой конфиг
    virtual void fromJson(const QJsonObject& obj) = 0;

    // Возвращает имя переменной, которая будет хранить этот объект
    virtual QString getName() = 0;
    
    // Устанавливает конфиг по умолчанию
    virtual void setDefaultConfig() = 0;

    virtual bool isJsonObjectValid(const QJsonObject& obj) = 0;

    // Получает текущий конфиг
    TConfig& getConfig() {
        return _config;
    }

    // Возвращает экземпляр класса
    static TDerived& instance() {
        static TDerived instance;
        return instance;
    }

};

#endif