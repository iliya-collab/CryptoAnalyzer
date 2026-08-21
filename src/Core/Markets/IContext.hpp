#pragma once
#include "Tools/StdTypes.hpp"

namespace Core::Markets
{
    class IContext : public QObject
    {
        Q_OBJECT
    public:

        IContext(QObject* parent = nullptr) : QObject(parent) {}

        // Загрузка информации об аккаунте
        virtual void requestInfoAboutAccount() = 0;
        // Загрузка споторых пар
        virtual void requestTradePairs() = 0;
        // Загрузка свеч
        virtual void requestKlines(const QString& symbol, const QString& interval, qint64 start, qint64 end) = 0;

    signals:

        void infoAboutAccountReceived(bool isValid);
        void klinesReceived(const QList<Core::Tools::Kline>& klines);
        void tradePairsReceived(const QList<Core::Tools::TradeInfo>& pairs);
    };
}