#pragma once

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include <expected>
#include <functional>

class DataBaseManager {
private:

    QSqlDatabase m_db;
    QString m_lastError;
    QString m_dbName;

    DataBaseManager();
    ~DataBaseManager();

public:

    static DataBaseManager& instance();

    DataBaseManager(const DataBaseManager&) = delete;
    DataBaseManager& operator=(const DataBaseManager&) = delete;

    QString error();
    bool existDBFile(const QString& name);
    bool open(const QString& name);
    void close();

    bool execInTransaction(const QStringList& queries);
    bool request(const QString& query, std::function<void(QSqlQuery&)> callback = nullptr);
    bool requestPrepared(const QString& query, const QVariant& value, std::function<void(QSqlQuery&)> callback = nullptr);
    bool requestPrepared(const QString& query, const QList<QVariant>& values, std::function<void(QSqlQuery&)> callback = nullptr);
};
