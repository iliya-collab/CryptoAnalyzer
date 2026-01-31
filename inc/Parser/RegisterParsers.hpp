#pragma once

#include <QHash>
#include <QSet>
#include <QString>

class RegisterParsers {
private:

    RegisterParsers() = default;
    
    QHash<QString, QSet<QString>> m_registry;

public:

    static RegisterParsers& instanse() {
        static RegisterParsers m_register;
        return m_register;
    }

    // Регистрация парсера с каналом
    void registerParser(const QString& name, const QString& channel);

    // Регистрация только парсера
    void registerParser(const QString& name);

    // Проверка наличия парсера с конкретным каналом
    bool hasRegistered(const QString& name, const QString& channel);

    // Проверка наличия парсера
    bool hasRegistered(const QString& name);

    // Проверка, есть ли у парсера каналы
    bool hasChannels(const QString& name);

    // Удаление канала у указанного парсера
    void deleteParser(const QString& name, const QString& channel);

    // Удаление парсера
    void deleteParser(const QString& name);

    // Получает список зарегестрированных каналов
    QSet<QString> getRegisteredChannels(const QString& name);

};