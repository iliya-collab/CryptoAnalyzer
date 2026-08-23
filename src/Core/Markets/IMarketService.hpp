#pragma once
#include "Tools/StdTypes.hpp"

namespace Core::Markets
{
    class IMarketService : public QObject
    {
        Q_OBJECT
    public:

        IMarketService(QObject* parent = nullptr) : QObject(parent) {}

        virtual void setAPI(const Tools::API& api) = 0;

        // Загрузка информации об аккаунте
        virtual void requestAccountBalance() = 0;
        // Загрузка споторых пар
        virtual void requestTradePairs() = 0;
        // Загрузка свеч
        virtual void requestKlines(const QString& symbol, const QString& interval, qint64 start, qint64 end) = 0;

    signals:

        void errorOccurred(const QString& error);
        void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

        void accountBalanceReceived(bool isValid);
        void klinesReceived(const QList<Core::Tools::Kline>& klines);
        void tradePairsReceived(const QList<Core::Tools::TradeInfo>& pairs);
    };
}