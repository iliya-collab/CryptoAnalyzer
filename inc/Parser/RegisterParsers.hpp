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

    // Регистрация с каналом
    void registerParser(const QString& name, const QString& channel);

    // Регистрация только ключа
    void registerParser(const QString& name);

    // Проверка наличия ключа с конкретным каналом
    bool hasRegistered(const QString& name, const QString& channel);

    // Проверка наличия ключа
    bool hasRegistered(const QString& name);

    // Проверка, есть ли у ключа значения
    bool hasChannels(const QString& name);

    // Удаление канала по ключу
    void deleteParser(const QString& name, const QString& channel);

    // Удаление ключа с каналами
    void deleteParser(const QString& name);

};