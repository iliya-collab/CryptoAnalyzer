#include "Engine/App/AppEngineLoader.hpp"

#include <QtConcurrent/QtConcurrent>

AppEngineLoader::LoadedData AppEngineLoader::getData() {
    return m_data;
}

void AppEngineLoader::startDownload() {
    QtConcurrent::run([this]() {
        runLoading();
    });
}

void AppEngineLoader::runStep(const LoadingStep& step) {
    try {
        qInfo().noquote() << step.name;
        step.action();
    } catch (const std::exception& e) {
        QMetaObject::invokeMethod(this, [this, msg = QString(e.what())]() { emit errorEngine(msg); });
        return;
    }
}

void AppEngineLoader::loadFromURLResources() {
    QVector<LoadingStep> steps = {
        {"Loading SPOT pairs",      [this]() { loadTradingPairsSync(Engine::TMarket::SPOT); }},
        {"Loading LINEAR pairs",    [this]() { loadTradingPairsSync(Engine::TMarket::LINEAR); }},
        {"Loading INVERSE pairs",   [this]() { loadTradingPairsSync(Engine::TMarket::INVERSE); }},
        {"Loading OPTION pairs",    [this]() { loadTradingPairsSync(Engine::TMarket::OPTION); }}
    };

    for (int i = 0; i < steps.size(); ++i) {
        QMetaObject::invokeMethod(this, [this, i, total = steps.size()]() { emit progressChanged(i + 1, total); });
        runStep(steps[i]);
    }
}

void AppEngineLoader::runLoading() {
    LoadingStep readConfig = { "Reading the platform configuration", [this]() {
        if (!Settings::readAllConfig())
            qDebug() << Settings::getLastError();
        m_api = PlatformConfig::instance().getConfig().m_api[0];
    }};
    runStep(readConfig);

    Engine::DBHash db;
    if (db.dbExist()) {
        qInfo() << "Loading from database";
        m_data.tradingPairs = db.getAllItems();
        qInfo() << "Loading from the database is completed";
    }
    else {
        loadFromURLResources();

        qInfo() << "Creating a database";
        db.create();
        qInfo() << "Database structure created successfully";

        qInfo() << "Filling the database";
        for (const auto& item : m_data.tradingPairs)
            db.addItem(item);
        qInfo() << "Database ready";
    }

    QMetaObject::invokeMethod(this, &AppEngineLoader::finished);
}

void AppEngineLoader::loadTradingPairsSync(Engine::TMarket market) {
    QDeadlineTimer deadline(LOADING_TIMEOUT);
    bool success = false;
    
    auto conn = connect(this, &AppEngineLoader::tradingPairsReady, [&](Engine::TMarket loadedType) {
        if (loadedType != market)
            return;
        success = true;
        qInfo().noquote() << QString("%1 pairs ready").arg(Engine::marketToString(market));
    });
    
    auto errorConn = connect(this, &AppEngineLoader::errorEngine, [&](const QString& error) {
        success = false;
    });
    
    getTradingPairs(market);
    
    while (!success && !deadline.hasExpired())
        QCoreApplication::processEvents();
    
    disconnect(conn);
    disconnect(errorConn);
    
    if (deadline.hasExpired())
        throw std::runtime_error("Timeout loading coins info");

    if (!success)
        throw std::runtime_error("Error loading trading pairs");
}

void AppEngineLoader::getTradingPairs(Engine::TMarket market) {
    Engine::BybitRestAPI* bybit_api = new Engine::BybitRestAPI(m_api);

    connect(bybit_api, &Engine::BybitRestAPI::dataReceived, this, [this, bybit_api, market] (const QJsonObject& data) {
        processSymbols(data);
        emit tradingPairsReady(market);
        bybit_api->deleteLater();
    }, Qt::QueuedConnection);

    connect(bybit_api, &Engine::BybitRestAPI::errorOccurred, this, [this, bybit_api] (const QString& error) {
        emit errorEngine(error);
        bybit_api->deleteLater();
    }, Qt::QueuedConnection);

    QUrlQuery params;
    params.addQueryItem("category", Engine::marketToString(market).toLower());
    bybit_api->requestEndpoint("/v5/market/instruments-info", params);
    
}

void AppEngineLoader::processSymbols(const QJsonObject& data) {
    QJsonObject result = data["result"].toObject();
    QString category = result["category"].toString();
    QJsonArray list = result["list"].toArray();

    for (const auto& obj : list) {
        QJsonObject item = obj.toObject();

        Engine::TradingInfo data;
        data.category = category;
        data.symbol = item["symbol"].toString();
        data.base_coin = item["baseCoin"].toString();
        data.quote_coin = item["quoteCoin"].toString();

        m_data.tradingPairs.append(data);
    }
}