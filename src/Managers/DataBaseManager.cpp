#include "Managers/DataBaseManager.hpp"
#include <QFileInfo>
#include <QThread>
#include <QDebug>

DataBaseManager::~DataBaseManager() {
    clear();
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

QSqlDatabase DataBaseManager::currentDatabase(const QString& name) {
    QString connectionName = QString("conn_%1").arg(quintptr(QThread::currentThreadId()));

    if (QSqlDatabase::contains(connectionName))
        return QSqlDatabase::database(connectionName);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    db.setDatabaseName(name);

    m_usedConnections.insert(connectionName, db);

    return db;
}

bool DataBaseManager::open(const QString& name) {
    QSqlDatabase db = currentDatabase(name);
    
    if (db.isOpen() && m_dbName != name)
        return false;
    
    if (db.isOpen() && m_dbName == name)
        return true;

    if (!db.open()) {
        m_lastError = QString("Error opening the database: %1").arg(db.lastError().text());
        return false;
    }

    m_dbName = name;

    return true;
}

void DataBaseManager::close() {
    QSqlDatabase db = currentDatabase(m_dbName);

    if (db.isOpen())
        db.close();
}

void DataBaseManager::clear() {
    if (m_usedConnections.isEmpty())
        return;

    for (auto [conn, db] : m_usedConnections.asKeyValueRange())
        if (db.isOpen())
            db.close();

    m_usedConnections.clear();
}


bool DataBaseManager::request(const QString& query, std::function<void(QSqlQuery&)> callback) {
    QSqlDatabase db = currentDatabase(m_dbName);
    
    if (!db.isOpen()) {
        m_lastError = "Database is not open";
        return false;
    }

    QSqlQuery sqlQuery(db);
    
    if (!sqlQuery.exec(query))  {
        m_lastError = QString("Processing error: %1").arg(sqlQuery.lastError().text());
        return false;
    }
    
    if (callback)
        callback(sqlQuery);

    return true;
}

bool DataBaseManager::requestPrepared(const QString& query, const QVariant& value, std::function<void(QSqlQuery&)> callback) {
    QSqlDatabase db = currentDatabase(m_dbName);

    if (!db.isOpen()) {
        m_lastError = "Database is not open";
        return false;
    }

    QSqlQuery sqlQuery(db);
    
    if (!sqlQuery.prepare(query)) {
        m_lastError = QString("Prepare error: %1").arg(sqlQuery.lastError().text());
        return false;
    }
    
    sqlQuery.addBindValue(value);
    
    if (!sqlQuery.exec()) {
        m_lastError = QString("Processing error: %1").arg(sqlQuery.lastError().text());
        return false;
    }
    
    if (callback)
        callback(sqlQuery);

    return true;
}

bool DataBaseManager::requestPrepared(const QString& query, const QList<QVariant>& values, std::function<void(QSqlQuery&)> callback) {
    QSqlDatabase db = currentDatabase(m_dbName);

    if (!db.isOpen()) {
        m_lastError = "Database is not open";
        return false;
    }

    QSqlQuery sqlQuery(db);
    
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
    QSqlDatabase db = currentDatabase(m_dbName);

    if (!db.isOpen()) {
        m_lastError = "Database is not open";
        return false;
    }

    QSqlQuery query(db);
    
    if (!query.exec("BEGIN IMMEDIATE TRANSACTION")) {
        m_lastError = QString("Failed to begin transaction: %1").arg(query.lastError().text());
        return false;
    }

    for (const QString& queryStr : queries) {
        if (queryStr.trimmed().isEmpty()) continue;
        
        if (!query.exec(queryStr)) {
            m_lastError = QString("Error executing query: %1\n%2").arg(queryStr.left(50)).arg(query.lastError().text());
            
            QSqlQuery rollbackQuery(db);
            if (!rollbackQuery.exec("ROLLBACK")) 
                qWarning() << "Failed to rollback transaction:" << rollbackQuery.lastError().text();
            return false;
        }
    }

    if (!query.exec("COMMIT")) {
        m_lastError = QString("Failed to commit transaction: %1").arg(query.lastError().text());
        QSqlQuery rollbackQuery(db);
        rollbackQuery.exec("ROLLBACK");
        return false;
    }

    return true;
}