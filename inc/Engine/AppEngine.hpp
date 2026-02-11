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
private:

    struct LoadedData {
        // TODO
    };

    Engine::API m_api;

    QSet<QString> m_usedCoins;

    QStringList processSymbols(const QJsonObject& data);
    void processInfoAboutCoins(const QJsonObject& data);

    void runSequentialLoading();

    void loadTradingPairsSync(Engine::TMarketData type);
    void loadCoinsInfoSync();

// ---------------------- Requests ----------------------

    // Получает доступные торговые пары на рынке
    // Вызывает сигнал spotReady или futuresReady
    void getTradingPairs(Engine::TMarketData market);

    // Получаем общую информацию о монетах (полное имя, описание, иконку)
    // Вызывает сигналы infoAboutCoinsReady и infoAboutIconsReady
    void getInfoAboutCoins();

    // Загружает все иконки
    // Вызывает сигнал iconsReady
    void downloadAllIcons(const QHash<QString, QString>& icons);

// ------------------------------------------------------

public: 

    AppEngine(QObject* parent = nullptr) : QObject(parent) {}

    void init();

    void startSequentialDownload();

    void setAPI(const Engine::API& api);

signals:

    // Отслеживания прогресса
    void progressChanged(int current, int total);
    // Окончание загрузки данных
    void finished();
    // Сообщения об ошибках
    void errorEngine(const QString& error);

    void infoAboutIconsReady(const QHash<QString, QString>& icons);
    void infoAboutCoinsReady(const QHash<QString, Engine::InfoAboutCoin>& lstCoins);
    void iconsReady(const QHash<QString, QByteArray>& icons);
    void tradingPairsReady(Engine::TMarketData market, const QStringList& pairs);

};