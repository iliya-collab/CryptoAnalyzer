#include "Managers/DataBaseManager.hpp"

#include <QFileInfo>

DataBaseManager::DataBaseManager() {
    m_db = QSqlDatabase::addDatabase("QSQLITE", "main_connection");
}

DataBaseManager::~DataBaseManager() {
    close();

    if (m_db.isValid()) {
        QString connectionName = m_db.connectionName();
        m_db = QSqlDatabase();
        QSqlDatabase::removeDatabase(connectionName);
    }
}

DataBaseManager& DataBaseManager::instance() {
    static DataBaseManager manager;
    return manager;
}

QString DataBaseManager::error() {
    return m_lastError;
}

bool DataBaseManager::existDBFile(const QString& name) {
    QFileInfo dbFile(name);
    return dbFile.exists();
}

bool DataBaseManager::open(const QString& name) {
    if (m_db.isOpen() && m_dbName != name)
        close();
    
    if (m_db.isOpen() && m_dbName == name)
        return true;

    m_db.setDatabaseName(name);
    
    if (!m_db.open()) {
        m_lastError = QString("Error opening the database: %1").arg(m_db.lastError().text());
        return false;
    }

    m_dbName = name;

    return true;
}

void DataBaseManager::close() {
    if (m_db.isOpen())
        m_db.close();
    m_dbName.clear();
}

bool DataBaseManager::request(const QString& query, std::function<void(const QSqlQuery&)> callback) {
    if (!m_db.isOpen()) {
        m_lastError = "Database is not open";
        return false;
    }

    QSqlQuery sqlQuery(m_db);
    
    if (!sqlQuery.exec(query))  {
        m_lastError = QString("Processing error: %1").arg(sqlQuery.lastError().text());
        return false;
    }
    
    if (callback)
        callback(sqlQuery);

    return true;
}

bool DataBaseManager::requestPrepared(const QString& query, const QList<QVariant>& values, std::function<void(const QSqlQuery&)> callback) {
    if (!m_db.isOpen()) {
        m_lastError = "Database is not open";
        return false;
    }

    QSqlQuery sqlQuery(m_db);
    
    if (!sqlQuery.prepare(query)) {
        m_lastError = QString("Prepare error: %1").arg(sqlQuery.lastError().text());
        return false;
    }
    
    for (const QVariant& value : values)
        sqlQuery.addBindValue(value);
    
    if (!sqlQuery.exec()) {
        m_lastError = QString("Processing error: %1").arg(sqlQuery.lastError().text());
        return false;
    }
    
    if (callback)
        callback(sqlQuery);

    return true;
}

bool DataBaseManager::execInTransaction(const QStringList& queries) {
    if (!m_db.isOpen()) {
        m_lastError = "Database is not open";
        return false;
    }

    QSqlQuery query(m_db);
    
    if (!query.exec("BEGIN IMMEDIATE TRANSACTION")) {
        m_lastError = QString("Failed to begin transaction: %1").arg(query.lastError().text());
        return false;
    }

    for (const QString& queryStr : queries) {
        if (queryStr.trimmed().isEmpty()) continue;
        
        if (!query.exec(queryStr)) {
            m_lastError = QString("Error executing query: %1\n%2")
                             .arg(queryStr.left(50))
                             .arg(query.lastError().text());
            
            QSqlQuery rollbackQuery(m_db);
            if (!rollbackQuery.exec("ROLLBACK")) 
                qWarning() << "Failed to rollback transaction:" << rollbackQuery.lastError().text();
            return false;
        }
    }

    if (!query.exec("COMMIT")) {
        m_lastError = QString("Failed to commit transaction: %1").arg(query.lastError().text());
        QSqlQuery rollbackQuery(m_db);
        rollbackQuery.exec("ROLLBACK");
        return false;
    }

    return true;
}