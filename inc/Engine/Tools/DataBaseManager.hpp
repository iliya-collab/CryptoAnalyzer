#pragma once

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QHash>
#include <QMutex>
#include <functional>

class DataBaseManager {
public:

    static DataBaseManager& instance();
    
    // Управление БД
    bool open(const QString& dbPath);
    bool close(const QString& dbPath);
    bool closeAll();
    bool isOpen(const QString& dbPath) const;
    QStringList openedDatabases() const;
    
    // Выполнение запросов для конкретной БД
    bool request(const QString& dbPath, const QString& query, std::function<void(QSqlQuery&)> callback = nullptr);
    bool requestPrepared(const QString& dbPath, const QString& query, const QVariant& value, std::function<void(QSqlQuery&)> callback = nullptr);
    bool requestPrepared(const QString& dbPath, const QString& query, const QList<QVariant>& values, std::function<void(QSqlQuery&)> callback = nullptr);

    // Транзакции для конкретной БД
    bool beginTransaction(const QString& dbPath);
    bool commitTransaction(const QString& dbPath);
    bool rollbackTransaction(const QString& dbPath);
    bool execInTransaction(const QString& dbPath, const QStringList& queries);
    bool rollbackAllTransactions();

    QString error() const;
    bool existDBFile(const QString& name);

private:

    DataBaseManager() = default;
    
    DataBaseManager(const DataBaseManager&) = delete;
    DataBaseManager& operator=(const DataBaseManager&) = delete;
    
    QSqlDatabase getDatabase(const QString& name);

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
