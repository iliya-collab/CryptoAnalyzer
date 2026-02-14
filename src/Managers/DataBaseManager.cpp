#include "Managers/DataBaseManager.hpp"

#include <QFileInfo>

DataBaseManager::DataBaseManager() {
    m_db = QSqlDatabase::addDatabase("QSQLITE");
}

DataBaseManager::~DataBaseManager() {
    close();
}

bool DataBaseManager::existDBFile(const QString& name) {
    QFileInfo dbFile(name);
    return dbFile.exists();
}

std::expected<bool, QString> DataBaseManager::open(const QString& name) {
    m_db.setDatabaseName(name);
    
    if (!m_db.open())
        return std::unexpected<QString>(QString("Error opening the database: %1").arg(m_db.lastError().text()));

    return true;
}

void DataBaseManager::close() {
    if (m_db.isOpen())
        m_db.close();
}

std::expected<bool, QString> DataBaseManager::request(const QString& query, std::function<void(const QSqlQuery&)> callback) {
    QSqlQuery sqlQuery;
    
    if (!sqlQuery.exec(query)) 
        return std::unexpected<QString>(QString("Processing error: %1").arg(sqlQuery.lastError().text()));
    
    if (callback)
        callback(sqlQuery);

    return true;
}