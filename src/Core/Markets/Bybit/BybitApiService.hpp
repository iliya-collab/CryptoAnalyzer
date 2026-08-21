#pragma once
#include "Markets/IMarketApiService.hpp"
#include "Tools/Network/BybitRestAPI.hpp"

namespace Core::Markets {

/*
 *  Класс для загрузки данных по сети (ByBit Rest API)
*/
    class BybitApiService : public IMarketApiService {
        Q_OBJECT
    private:

        std::unique_ptr<Tools::BybitRestAPI> m_currentApi;
        QMap<QUrl, std::function<void(const QJsonObject&)>> m_requestedUrls;
        mutable QMutex m_mutex;
        int LOADING_TIMEOUT = 30000;

        void processRequestTradePairs(QList<Tools::TradeInfo>& pairs, const QJsonObject& data);
        void processRequestKlines(QList<Tools::Kline>& klines, const QString& interval, const QJsonObject& data);

    private slots:

        void onDataReceived(const QUrl& reqUrl, const QJsonObject& data);
        void onInfoAboutAccountReceived(const QJsonObject& data);
        void onTradePairsReceived(const QJsonObject& data);
        void onKlinesReceived(const QJsonObject& data);

    public:

        explicit BybitApiService(QObject* parent = nullptr);

        void setAPI(const Tools::API& api) override;

        // Загрузка информации об аккаунте
        void requestInfoAboutAccount() override;
        // Загрузка споторых пар
        void requestTradePairs() override;
        // Загрузка свеч
        void requestKlines(const QString& symbol, const QString& interval, qint64 start, qint64 end) override;

    // signals:
    //     void errorOccurred(const QString& error);
    //     void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    //     void infoAboutAccountReceived(bool isValid);
    //     void klinesReceived(const QList<Tools::Kline>& klines);
    //     void tradePairsReceived(const QList<Tools::TradeInfo>& pairs);

    };

}

