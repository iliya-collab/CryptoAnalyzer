#pragma once
#include "../Database/IDatabaseManager.hpp"
#include <QString>

namespace Core::Tools {

    class BaseRepository {

    protected:

        QString m_dbPath;
        IDatabaseManager& m_dbManager;

    public:

        virtual bool init() = 0;
        virtual bool open() = 0;
        virtual void close() = 0;
        virtual QString error() = 0;
        virtual bool clear() = 0;

        BaseRepository(const QString& dbPath, IDatabaseManager& manager) :
            m_dbPath(dbPath), m_dbManager(manager) {};
        virtual ~BaseRepository() {}

    };

}