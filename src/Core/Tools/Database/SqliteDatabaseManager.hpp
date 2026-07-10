#pragma once

#include "IDatabaseManager.hpp"
#include <QSqlDatabase>
#include <QSqlError>
#include <QHash>
#include <QMutex>

namespace Core::Tools {


    class SqliteDatabaseManager : public IDatabaseManager {
    public:

        static SqliteDatabaseManager& instance();

        // Управление БД
        bool open(const QString& dbPath) override;
        void close(const QString& dbPath) override;
        bool closeAll() override;
        bool isOpen(const QString& dbPath) const override;
        QStringList openedDatabases() const override;
        QString error() const override;
        bool existDBFile(const QString& name) const override;

        // Выполнение запросов для конкретной БД
        bool executeQuery(const QString& dbPath, const QString& query, const std::function<void(QSqlQuery&)>& callback = nullptr) override;
        bool executePrepared(const QString& dbPath, const QString& query, const QVariantList& values, const std::function<void(QSqlQuery&)>& callback = nullptr) override;

        // Транзакции для конкретной БД
        bool beginTransaction(const QString& dbPath) override;
        bool commitTransaction(const QString& dbPath) override;
        bool rollbackTransaction(const QString& dbPath) override;
        bool executeTransaction(const QString& dbPath, const QStringList& queries) override;
        bool rollbackAllTransactions() override;

    private:

        SqliteDatabaseManager() = default;

        SqliteDatabaseManager(const SqliteDatabaseManager&) = delete;
        SqliteDatabaseManager& operator=(const SqliteDatabaseManager&) = delete;

        QSqlDatabase getDatabase(const QString& dbPath);

        struct DatabaseInfo {
            QSqlDatabase database;
            QString connectionName;
            bool inTransaction = false;
            int transactionDepth = 0;
        };

        QHash<QString, DatabaseInfo> m_databases;
        QString m_lastError;
        mutable QMutex m_mutex;
    };

}
