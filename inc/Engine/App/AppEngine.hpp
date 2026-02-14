#pragma once

#include "Configs/PlatformConfig.hpp"
#include "Managers/Settings.hpp"

#include "Engine/StdTypes.hpp"
#include "Engine/WebSocket.hpp"
#include "Engine/WebSocketBuilder.hpp"
#include "Engine/BybitRestAPI.hpp"
#include "Engine/CryptoCompare.hpp"
#include "Engine/LocalHash.hpp"
#include "Engine/DBHash.hpp"

#include <QHash>
#include <memory>

class AppEngine : public QObject {
    Q_OBJECT
private:

    struct LoadedData {
        QSet<QString> usedCoins;
        QHash<Engine::TMarket, QStringList> tradingPairs;
        QList<Engine::InfoAboutCoin> infoAboutCoins;
    };

    struct LoadingStep {
        QString name;
        std::function<void()> action;
    };

    Engine::API m_api;

    LoadedData m_data;

    const qint64 LOADING_TIMEOUT = 60000; 

    QStringList processSymbols(const QJsonObject& data);
    void processInfoAboutCoins(const QJsonObject& data);

    void runLoading();

    void loadFromURLResources();
    void loadFromDB(const DBHash& db_hash);

    void loadTradingPairsSync(Engine::TMarket market);
    void loadCoinsInfoSync();
    void loadIconsCoinSync();

    // Получает доступные торговые пары на рынке
    // tradingPairsReady
    void getTradingPairs(Engine::TMarket market);

    // Получаем общую информацию о монетах (полное имя, описание, иконку)
    // infoAboutCoinsReady
    void getInfoAboutCoins();

    // Загружает все иконки
    // infoAboutIconsReady
    void downloadAllIcons();

public: 

    AppEngine(QObject* parent = nullptr) : QObject(parent) {}

    void init();

    void startDownload();

    void setAPI(const Engine::API& api);

signals:

    // Отслеживания прогресса
    void progressChanged(int current, int total);
    // Окончание загрузки данных
    void finished();
    // Сообщения об ошибках
    void errorEngine(const QString& error);

    void tradingPairsReady(Engine::TMarket market);
    void infoAboutCoinsReady();
    void infoAboutIconsReady();

};