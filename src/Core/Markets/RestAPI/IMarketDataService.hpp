#pragma once
#include "Tools/StdTypes.hpp"

namespace Core::Markets
{
    class IMarketDataService : public QObject
    {
        Q_OBJECT
    public:

        IMarketDataService(QObject* parent = nullptr) : QObject(parent) {}

        virtual void init(bool isTestnet) = 0;

        // Запрос пар
        virtual void requestTradePairs(Tools::MarketType category) = 0;
        // Запрос свеч
        virtual void requestKlines(Tools::MarketType category, const QString& symbol, const QString& interval, qint64 start, qint64 end) = 0;

    signals:

        void errorOccurred(const QString& error);
        void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
        void klinesReceived(const QList<Core::Tools::Kline>& klines);
        void tradePairsReceived(const QList<Core::Tools::TradeInfo>& pairs);

    };
}