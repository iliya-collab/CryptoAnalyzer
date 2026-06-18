#pragma once

#include "CryptoRepository.hpp"
#include "CandleRepository.hpp"
#include <memory>

namespace Engine {

    class RepositoryCreater {
    public:

        static RepositoryCreater& instance();

        std::unique_ptr<CryptoRepository> createCryptoRepository(const QString& dbPath, IDatabaseManager& dbManager) const;
        std::unique_ptr<CandleRepository> createCandleRepository(const QString& dbPath, IDatabaseManager& dbManager) const;

    private:

        RepositoryCreater() = default;
        RepositoryCreater(const RepositoryCreater&) = delete;
        RepositoryCreater& operator=(const RepositoryCreater&) = delete;

    };

}
