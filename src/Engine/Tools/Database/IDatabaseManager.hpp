#pragma once

#include <functional>
#include <QString>
#include <QSqlQuery>

namespace Engine {

    class IDatabaseManager {
    public:
        // Управление БД
        virtual bool open(const QString& dbPath) = 0;
        virtual void close(const QString& dbPath) = 0;
        virtual bool closeAll() = 0;
        virtual bool isOpen(const QString& dbPath) const = 0;
        virtual QStringList openedDatabases() const = 0;
        virtual QString error() const = 0;
        virtual bool existDBFile(const QString& name) const = 0;

        // Выполнение запросов для конкретной БД
        virtual bool executeQuery(const QString& dbPath, const QString& query, const std::function<void(QSqlQuery&)>& callback = nullptr) = 0;
        virtual bool executePrepared(const QString& dbPath, const QString& query, const QVariantList& values, const std::function<void(QSqlQuery&)>& callback = nullptr) = 0;

        // Транзакции для конкретной БД
        virtual bool beginTransaction(const QString& dbPath) = 0;
        virtual bool commitTransaction(const QString& dbPath) = 0;
        virtual bool rollbackTransaction(const QString& dbPath) = 0;
        virtual bool executeTransaction(const QString& dbPath, const QStringList& queries) = 0;
        virtual bool rollbackAllTransactions() = 0;
    };

}
