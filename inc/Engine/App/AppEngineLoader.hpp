#pragma once

#include "Configs/PlatformConfig.hpp"
#include "Managers/Settings.hpp"

#include "Engine/StdTypes.hpp"
#include "Engine/WebSocket.hpp"
#include "Engine/WebSocketBuilder.hpp"
#include "Engine/BybitRestAPI.hpp"
#include "Engine/DBHash.hpp"

#include <QHash>
#include <memory>

class AppEngineLoader : public QObject {
    Q_OBJECT
private:

    struct LoadedData {
        QList<Engine::TradingInfo> tradingPairs;
    };

    struct LoadingStep {
        QString name;
        std::function<void()> action;
    };

    Engine::API m_api;

    LoadedData m_data;

    const qint64 LOADING_TIMEOUT = 60000; 

    void processSymbols(const QJsonObject& data);

    void runLoading();

    void loadFromURLResources();
    void runStep(const LoadingStep& step);

    void loadTradingPairsSync(Engine::TMarket market);

    // Получает доступные торговые пары на рынке
    // tradingPairsReady
    void getTradingPairs(Engine::TMarket market);

public: 

    AppEngineLoader(QObject* parent = nullptr) : QObject(parent) {}

    void startDownload();

    LoadedData getData();

signals:

    // Отслеживания прогресса
    void progressChanged(int current, int total);
    // Окончание загрузки данных
    void finished();
    // Сообщения об ошибках
    void errorEngine(const QString& error);

    void tradingPairsReady(Engine::TMarket market);

};