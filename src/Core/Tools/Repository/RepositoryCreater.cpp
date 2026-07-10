#include "RepositoryCreater.hpp"

namespace Core::Tools {

    RepositoryCreater& RepositoryCreater::instance() {
        static RepositoryCreater creater;
        return creater;
    }

    std::unique_ptr<CryptoRepository> RepositoryCreater::createCryptoRepository(const QString& dbPath, IDatabaseManager& dbManager) const {
        return std::make_unique<CryptoRepository>(dbPath, dbManager);
    }

    std::unique_ptr<CandleRepository> RepositoryCreater::createCandleRepository(const QString& dbPath, IDatabaseManager& dbManager) const {
        return std::make_unique<CandleRepository>(dbPath, dbManager);
    }

}
