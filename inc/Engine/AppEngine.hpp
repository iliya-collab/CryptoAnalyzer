#pragma once

#include "Configs/PlatformConfig.hpp"
#include "Managers/Settings.hpp"

#include "Engine/StdTypes.hpp"
#include "Engine/WebSocket.hpp"
#include "Engine/WebSocketBuilder.hpp"
#include "Engine/BybitRestAPI.hpp"
#include "Engine/CryptoCompare.hpp"
#include "Engine/LocalHash.hpp"

#include <QHash>
#include <memory>

class AppEngine : public QObject {
    Q_OBJECT
public:
    
    struct EngineLaunchParams {
        QString m_stockMarket;
        QString m_market;
        QString m_coin;
    };

private:

    std::unique_ptr<Engine::WebSocket> m_webSocket = nullptr;

    Engine::API m_api;
    EngineLaunchParams m_launch;

    QSet<QString> m_usedCoins;

    void processSymbols(const QJsonObject& data);
    void processInfoAboutCoins(const QJsonObject& data);

public: 

    AppEngine(QObject* parent = nullptr) : QObject(parent) {}

    void init();

    void setAPI(const Engine::API& api);
    void setLaunchParams(const EngineLaunchParams& launch);

// ---------------------- Requests ----------------------


    // Получает доступные торговые пары на споте
    // Вызывает сигнал spotReady
    void getSpotTradingPairs();

    // Получает доступные контракты на фьючерсах
    // Вызывает сигнал futuresReady
    void getFuturesContracts();

    // Получаем общую информацию о монетах (полное имя, описание, иконку)
    // Вызывает сигналы infoAboutCoinsReady и infoAboutIconsReady
    void getInfoAboutCoins();

    // Загружает все иконки
    // Вызывает сигнал iconsReady
    void downloadAllIcons(const QHash<QString, QString>& icons);

// ------------------------------------------------------

signals:

    void infoAboutCoinsReady(const QHash<QString, Engine::InfoAboutCoin>& lstCoins);
    void infoAboutIconsReady(const QHash<QString, QString>& icons);

    void iconsReady(const QHash<QString, QByteArray>& icons);
    void spotReady(const QStringList& pairs);
    void futuresReady(const QStringList& contracts);

};