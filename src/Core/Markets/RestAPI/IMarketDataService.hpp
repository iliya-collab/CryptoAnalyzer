#pragma once
#include "Tools/StdTypes.hpp"

namespace Core::Markets
{
    class IMarketDataService : public QObject
    {
        Q_OBJECT
    public:

        IMarketDataService(QObject* parent = nullptr) : QObject(parent) {}

        virtual void setApi(const Tools::Api& api) = 0;

        // Запрос информации об аккаунте
        virtual void requestAccountBalance() = 0;
        // Запрос информации об Api
        virtual void requestInfoAboutApi() = 0;
        // Запрос споторых пар
        virtual void requestTradePairs(Tools::MarketType category) = 0;
        // Запрос свеч
        virtual void requestKlines(Tools::MarketType category, const QString& symbol, const QString& interval, qint64 start, qint64 end) = 0;

    signals:

        void errorOccurred(const QString& error);
        void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

        void accountBalanceReceived(const Core::Tools::AccountBalance& balance);
        void accountVerified();
        void infoAboutApiReceived(const Core::Tools::ApiInfo& apiInfo);
        void klinesReceived(const QList<Core::Tools::Kline>& klines);
        void tradePairsReceived(const QList<Core::Tools::TradeInfo>& pairs);

    };
}