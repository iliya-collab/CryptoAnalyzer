#pragma once

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include <expected>
#include <functional>

class DataBaseManager {
private:
    QSqlDatabase m_db;

public:
    DataBaseManager();
    ~DataBaseManager();

    bool existDBFile(const QString& name);

    std::expected<bool, QString> open(const QString& name);
    void close();
    
    std::expected<bool, QString> request(const QString& query, std::function<void(const QSqlQuery&)> callback = nullptr);
};
