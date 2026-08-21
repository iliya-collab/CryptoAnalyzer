#pragma once

#include "BaseRepository.hpp"
#include <type_traits>
#include <memory>

namespace Core::Tools {

    class RepositoryCreater {
    public:

        static RepositoryCreater& instance();

        template <class TRep>
        std::unique_ptr<TRep> createRepository(const QString& dbPath, IDatabaseManager& dbManager) const {
            static_assert(std::is_base_of<BaseRepository, TRep>::value, "TRep must inherit from BaseRepository!");

            auto rep = std::make_unique<TRep>(dbPath, dbManager);

            if (!rep->init())
                throw std::invalid_argument("Failed to initialize the repository");

            return rep;
        }

    private:

        RepositoryCreater() = default;
        RepositoryCreater(const RepositoryCreater&) = delete;
        RepositoryCreater& operator=(const RepositoryCreater&) = delete;

    };

}
