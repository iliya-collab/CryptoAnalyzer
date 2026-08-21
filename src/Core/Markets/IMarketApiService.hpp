#pragma once
#include <Tools/StdTypes.hpp>
#include <QObject>

namespace Core::Markets {

/*
 *  Класс для загрузки данных по сети
*/
class IMarketApiService : public QObject {
    Q_OBJECT
public:

    explicit IMarketApiService(QObject* parent = nullptr) : QObject(parent) {};
    virtual ~IMarketApiService() {}

    virtual void setAPI(const Tools::API& api) = 0;
    // Загрузка информации об аккаунте
    virtual void requestInfoAboutAccount() = 0;
    // Загрузка споторых пар
    virtual void requestTradePairs() = 0;
    // Загрузка свеч
    virtual void requestKlines(const QString& symbol, const QString& interval, qint64 start, qint64 end) = 0;

signals:
    void errorOccurred(const QString& error);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void infoAboutAccountReceived(bool isValid);
    void klinesReceived(const QList<Tools::Kline>& klines);
    void tradePairsReceived(const QList<Tools::TradeInfo>& pairs);

};

}

