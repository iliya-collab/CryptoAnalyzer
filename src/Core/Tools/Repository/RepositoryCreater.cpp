#include "RepositoryCreater.hpp"

namespace Core::Tools {

    RepositoryCreater& RepositoryCreater::instance() {
        static RepositoryCreater creater;
        return creater;
    }

}
