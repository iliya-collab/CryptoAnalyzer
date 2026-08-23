#pragma once
#include "Markets/BaseMarketApiService.hpp"

namespace Core::Markets {

/*
 *  Класс для загрузки данных по сети (ByBit Rest API)
*/
    class BybitApiService : public BaseMarketApiService {
        Q_OBJECT
    public:

        explicit BybitApiService(QObject* parent = nullptr);

        void setAPI(const Tools::API& api) override;

        // Методы IContext
        void requestAccountBalance() override;
        void requestTradePairs() override;
        void requestKlines(const QString& symbol, const QString& interval, qint64 start, qint64 end) override;

    };

}

