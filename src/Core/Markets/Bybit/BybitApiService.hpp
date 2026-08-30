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

        void setApi(const Tools::Api& api) override;

        // Методы IMarketService
        void requestInfoAboutApi() override;
        void requestAccountBalance() override;
        void requestTradePairs() override;
        void requestKlines(const QString& symbol, const QString& interval, qint64 start, qint64 end) override;

    };

}

