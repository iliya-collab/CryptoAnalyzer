#include "SqliteDatabaseManager.hpp"
#include <QFileInfo>
#include <QThread>
#include <QDebug>
#include <QDir>

namespace Core::Tools {

    SqliteDatabaseManager& SqliteDatabaseManager::instance() {
        static SqliteDatabaseManager manager;
        return manager;
    }

    QString SqliteDatabaseManager::error() const {
        QMutexLocker locker(&m_mutex);
        return m_lastError;
    }

    bool SqliteDatabaseManager::existDBFile(const QString& name) const {
        QFileInfo dbFile(name);
        return dbFile.exists();
    }

    bool SqliteDatabaseManager::open(const QString& dbPath) {
        QMutexLocker locker(&m_mutex);

        if (!existDBFile(dbPath)) {
            QDir dir = QFileInfo(dbPath).absoluteDir();
            if (!dir.exists())
                dir.mkpath(".");
        }

        if (m_databases.contains(dbPath)) {
            DatabaseInfo& info = m_databases[dbPath];
            if (info.database.isOpen())
                return true;
        }

        QSqlDatabase db = getDatabase(dbPath);

        if (!db.open()) {
            m_lastError = QString("Error opening database %1: %2").arg(dbPath).arg(db.lastError().text());
            return false;
        }

        m_databases[dbPath] = {db, db.connectionName(), false};
        return true;
    }

    void SqliteDatabaseManager::close(const QString& dbPath) {
        QMutexLocker locker(&m_mutex);

        if (!m_databases.contains(dbPath))
            return;

        DatabaseInfo& info = m_databases[dbPath];
        if (info.database.isOpen())
            info.database.close();

        if (QSqlDatabase::contains(info.connectionName))
            QSqlDatabase::removeDatabase(info.connectionName);

        m_databases.remove(dbPath);
    }

    bool SqliteDatabaseManager::closeAll() {
        QMutexLocker locker(&m_mutex);

        for (auto it = m_databases.begin(); it != m_databases.end(); ++it) {
            if (it->database.isOpen())
                it->database.close();

            if (QSqlDatabase::contains(it->connectionName))
                QSqlDatabase::removeDatabase(it->connectionName);
        }

        m_databases.clear();

        return true;
    }

    bool SqliteDatabaseManager::isOpen(const QString& dbPath) const {
        QMutexLocker locker(&m_mutex);
        return m_databases.contains(dbPath) && m_databases[dbPath].database.isOpen();
    }

    QStringList SqliteDatabaseManager::openedDatabases() const {
        QMutexLocker locker(&m_mutex);
        return m_databases.keys();
    }

    bool SqliteDatabaseManager::executeQuery(const QString& dbPath, const QString& query, const std::function<void(QSqlQuery&)>& callback) {
        QMutexLocker locker(&m_mutex);

        if (!m_databases.contains(dbPath)) {
            m_lastError = QString("Database %1 is not opened").arg(dbPath);
            return false;
        }

        QSqlDatabase db = m_databases[dbPath].database;
        if (!db.isOpen()) {
            m_lastError = QString("Database %1 is not open").arg(dbPath);
            return false;
        }

        QSqlQuery sqlQuery(db);

        if (!sqlQuery.exec(query)) {
            m_lastError = QString("Query error on %1: %2").arg(dbPath).arg(sqlQuery.lastError().text());
            return false;
        }

        if (callback)
            callback(sqlQuery);

        return true;
    }

    bool SqliteDatabaseManager::executePrepared(const QString& dbPath, const QString& query, const QVariantList& values, const std::function<void(QSqlQuery&)>& callback) {
        QMutexLocker locker(&m_mutex);

        if (!m_databases.contains(dbPath)) {
            m_lastError = QString("Database %1 is not opened").arg(dbPath);
            return false;
        }

        QSqlDatabase db = m_databases[dbPath].database;
        if (!db.isOpen()) {
            m_lastError = QString("Database %1 is not open").arg(dbPath);
            return false;
        }
        QSqlQuery sqlQuery(db);

        if (!sqlQuery.prepare(query)) {
            m_lastError = QString("Prepare error on %1: %2").arg(dbPath).arg(sqlQuery.lastError().text());
            return false;
        }

        for (const QVariant& value : values)
            sqlQuery.addBindValue(value);

        if (!sqlQuery.exec()) {
            m_lastError = QString("Processing error on %1: %2").arg(dbPath).arg(sqlQuery.lastError().text());
            return false;
        }

        if (callback)
            callback(sqlQuery);

        return true;
    }

    bool SqliteDatabaseManager::rollbackTransaction(const QString& dbPath) {
        QMutexLocker locker(&m_mutex);

        if (!m_databases.contains(dbPath)) {
            m_lastError = QString("Database %1 is not opened").arg(dbPath);
            return false;
        }

        DatabaseInfo& info = m_databases[dbPath];

        if (!info.inTransaction) {
            m_lastError = QString("No active transaction to rollback in database %1").arg(dbPath);
            return false;
        }

        if (!info.database.isOpen()) {
            m_lastError = QString("Database %1 connection is closed").arg(dbPath);
            info.inTransaction = false;
            info.transactionDepth = 0;
            return false;
        }

        QSqlQuery query(info.database);
        if (!query.exec("ROLLBACK")) {
            m_lastError = QString("Failed to rollback transaction in database %1: %2").arg(dbPath).arg(query.lastError().text());
            return false;
        }

        info.inTransaction = false;
        info.transactionDepth = 0;

        return true;
    }

    bool SqliteDatabaseManager::beginTransaction(const QString& dbPath) {
        QMutexLocker locker(&m_mutex);

        if (!m_databases.contains(dbPath)) {
            m_lastError = QString("Database %1 is not opened").arg(dbPath);
            return false;
        }

        DatabaseInfo& info = m_databases[dbPath];

        if (info.inTransaction) {
            QString savepointName = QString("savepoint_%1").arg(info.transactionDepth);
            QSqlQuery query(info.database);

            if (!query.exec(QString("SAVEPOINT %1").arg(savepointName))) {
                m_lastError = QString("Failed to create savepoint: %1").arg(query.lastError().text());
                return false;
            }

            info.transactionDepth++;
            return true;
        }

        QSqlQuery query(info.database);
        if (!query.exec("BEGIN IMMEDIATE TRANSACTION")) {
            m_lastError = QString("Failed to begin transaction: %1").arg(query.lastError().text());
            return false;
        }

        info.inTransaction = true;
        info.transactionDepth = 1;

        return true;
    }

    bool SqliteDatabaseManager::commitTransaction(const QString& dbPath) {
        QMutexLocker locker(&m_mutex);

        if (!m_databases.contains(dbPath)) {
            m_lastError = QString("Database %1 is not opened").arg(dbPath);
            return false;
        }

        DatabaseInfo& info = m_databases[dbPath];

        if (!info.inTransaction) {
            m_lastError = QString("No active transaction to commit in database %1").arg(dbPath);
            return false;
        }

        if (info.transactionDepth > 1) {
            QString savepointName = QString("savepoint_%1").arg(info.transactionDepth - 1);
            QSqlQuery query(info.database);

            if (!query.exec(QString("RELEASE SAVEPOINT %1").arg(savepointName))) {
                m_lastError = QString("Failed to release savepoint: %1")
                .arg(query.lastError().text());
                return false;
            }

            info.transactionDepth--;
            return true;
        }

        QSqlQuery query(info.database);
        if (!query.exec("COMMIT")) {
            m_lastError = QString("Failed to commit transaction: %1")
            .arg(query.lastError().text());

            query.exec("ROLLBACK");
            info.inTransaction = false;
            info.transactionDepth = 0;
            return false;
        }

        info.inTransaction = false;
        info.transactionDepth = 0;

        return true;
    }

    bool SqliteDatabaseManager::executeTransaction(const QString& dbPath, const QStringList& queries) {
        if (!beginTransaction(dbPath)) {
            m_lastError = "Failed to begin transaction: " + m_lastError;
            return false;
        }

        int executedQueries = 0;
        for (const QString& queryStr : queries)
            if (!queryStr.trimmed().isEmpty()) {
                if (!executeQuery(dbPath, queryStr)) {
                    rollbackTransaction(dbPath);
                    return false;
                }
                executedQueries++;
            }


        if (!commitTransaction(dbPath)) {
            m_lastError = "Failed to commit transaction: " + m_lastError;
            rollbackTransaction(dbPath);
            return false;
        }

        return true;
    }

    bool SqliteDatabaseManager::rollbackAllTransactions() {
        QMutexLocker locker(&m_mutex);
        bool allSuccess = true;

        for (auto it = m_databases.begin(); it != m_databases.end(); ++it) {
            if (it->inTransaction) {
                QString dbPath = it.key();
                QSqlQuery query(it->database);

                if (!query.exec("ROLLBACK"))
                    allSuccess = false;
                else {
                    it->inTransaction = false;
                    it->transactionDepth = 0;
                }
            }
        }

        if (allSuccess)
            m_lastError.clear();
        else
            m_lastError = "Failed to rollback some transactions";


        return allSuccess;
    }

    QSqlDatabase SqliteDatabaseManager::getDatabase(const QString& dbPath) {
        QString connectionName = QString("%1_conn_%2").arg(QFileInfo(dbPath).baseName()).arg(quintptr(QThread::currentThreadId()));

        if (QSqlDatabase::contains(connectionName))
            return QSqlDatabase::database(connectionName);

        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        db.setDatabaseName(dbPath);
        return db;
    }

}