#pragma once
#include "BaseRepository.hpp"
#include "../StdTypes.hpp"
#include "../Database/IDatabaseManager.hpp"

namespace Core::Tools {

    class KlineRepository : public BaseRepository {
    public:

        KlineRepository(const QString& dbPath, IDatabaseManager& manager);
        ~KlineRepository() = default;

        bool init() override;
        bool open() override;
        void close() override;
        QString error() override;
        bool clear() override;

        bool insertKlines(const QList<Kline>& newKlines);
        bool insertKline(const Kline& newKline);

    };

}