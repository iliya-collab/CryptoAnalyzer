#pragma once

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QHash>
#include <functional>

class DataBaseManager {
public:

    static DataBaseManager& instance();
    
    QString error();
    bool existDBFile(const QString& name);
    bool open(const QString& name);
    void close();

    void clear();
    
    bool request(const QString& query, std::function<void(QSqlQuery&)> callback = nullptr);
    bool requestPrepared(const QString& query, const QVariant& value, std::function<void(QSqlQuery&)> callback = nullptr);
    bool requestPrepared(const QString& query, const QList<QVariant>& values, std::function<void(QSqlQuery&)> callback = nullptr);
    bool execInTransaction(const QStringList& queries);

private:

    DataBaseManager() = default;
    ~DataBaseManager();
    
    DataBaseManager(const DataBaseManager&) = delete;
    DataBaseManager& operator=(const DataBaseManager&) = delete;
    
    QSqlDatabase currentDatabase(const QString& name);
    
    QHash<QString, QSqlDatabase> m_usedConnections;

    QString m_lastError;
    QString m_dbName;
};
