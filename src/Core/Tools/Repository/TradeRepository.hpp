#pragma once
#include "BaseRepository.hpp"

namespace Core::Tools {

    class TradeRepository : public BaseRepository {
    public:

        TradeRepository(const QString& dbPath, IDatabaseManager& manager);
        ~TradeRepository() = default;

        bool init() override;
        bool open() override;
        void close() override;
        QString error() override;
        bool clear() override;

    };
}
