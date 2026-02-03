#pragma once

#include <QHash>
#include <QSet>
#include <QString>

namespace Engine {

    class Registry {
    private:

        Registry() = default;
        
        QHash<QString, QSet<QString>> m_registry;

    public:

        static Registry& instanse() {
            static Registry m_reg;
            return m_reg;
        }

        // Регистрация биржи/рынок и монеты
        void Register(const QString& name, const QString& coin);
        // Регистрация биржи/рынок
        void Register(const QString& name);

        // Проверка наличия биржи/рынок с конкретным монетой
        bool hasRegistered(const QString& name, const QString& coin);
        // Проверка наличия биржи/рынок
        bool hasRegistered(const QString& name);
        // Проверка наличия у биржи/рынок монет
        bool hasCoins(const QString& name);

        // Удаление монеты у указанной биржи/рынок
        void Delete(const QString& name, const QString& coin);
        // Удаление биржи/рынок
        void Delete(const QString& name);

        // Получает список зарегестрированных монет биржи/рынок
        QSet<QString> getRegisteredCoins(const QString& name);

    };

}