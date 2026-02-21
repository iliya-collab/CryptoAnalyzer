#include "Engine/App/AppEngineLoader.hpp"

#include <QtConcurrent/QtConcurrent>

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
        {"Loading OPTION pairs",    [this]() { loadTradingPairsSync(Engine::TMarket::OPTION); }},
        {"Loading coins info",      [this]() { loadCoinsInfoSync(); }}
    };

    for (int i = 0; i < steps.size(); ++i) {
        QMetaObject::invokeMethod(this, [this, i, total = steps.size()]() { emit progressChanged(i + 1, total); });
        runStep(steps[i]);
    }
}

void AppEngineLoader::loadFromDB(const Engine::DBHash& db_hash) {

}

void AppEngineLoader::runLoading() {
    LoadingStep readConfig = { "Reading the platform configuration", [this]() {
        if (!Settings::readAllConfig())
            qDebug() << Settings::getLastError();
    }};
    runStep(readConfig);

    Engine::DBHash db;
    if (db.dbExist())
        loadFromDB(db);
    else {
        loadFromURLResources();

        db.create();
        for (const auto& item : m_data.infoAboutCoins)
            db.addItem(Engine::convertTo(item));
        for (const auto& item : m_data.tradingPairs)
            db.addItem(Engine::convertTo(item));
    }

    LoadingStep loadIcons = { "Loading icons coin", [this]() {
        loadIconsCoinSync();
    }};
    runStep(loadIcons);

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

void AppEngineLoader::loadCoinsInfoSync() {
    QDeadlineTimer deadline(LOADING_TIMEOUT);
    bool success = false;
    
    auto conn = connect(this, &AppEngineLoader::infoAboutCoinsReady, [&]() {
        success = true;
        qInfo().noquote() << "Coins info ready";
    });
    
    auto errorConn = connect(this, &AppEngineLoader::errorEngine, [&](const QString&) {
        success = false;
    });
    
    getInfoAboutCoins();
    
    while (!success && !deadline.hasExpired())
        QCoreApplication::processEvents();

    disconnect(conn);
    disconnect(errorConn);
    
    if (deadline.hasExpired())
        throw std::runtime_error("Timeout loading coins info");

    if (!success)
        throw std::runtime_error("Error loading coins info");
}

void AppEngineLoader::loadIconsCoinSync() {
    QDeadlineTimer deadline(-1);
    bool success = false;
    
    auto conn = connect(this, &AppEngineLoader::infoAboutIconsReady, [&]() {
        success = true;
        qInfo().noquote() << "Icons coin ready";
    });
    
    auto errorConn = connect(this, &AppEngineLoader::errorEngine, [&](const QString&) {
        success = false;
    });
    
    downloadIcons();
    
    while (!success && !deadline.hasExpired())
        QCoreApplication::processEvents();

    disconnect(conn);
    disconnect(errorConn);
    
    if (deadline.hasExpired())
        throw std::runtime_error("Timeout loading icons coin");

    if (!success)
        throw std::runtime_error("Error loading icons coin");
}

void AppEngineLoader::setAPI(const Engine::API& api) {
    m_api = api;
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

void AppEngineLoader::getInfoAboutCoins() {
    Engine::CryptoCompare* crypto_compare = new Engine::CryptoCompare;
    
    connect(crypto_compare, &Engine::CryptoCompare::dataReceived, this, [this, crypto_compare] (const QJsonObject& data) {
        processInfoAboutCoins(data);
        emit infoAboutCoinsReady();
        crypto_compare->deleteLater();
    }, Qt::QueuedConnection);

    connect(crypto_compare, &Engine::CryptoCompare::errorOccurred, this, [this, crypto_compare] (const QString& error) {
        emit errorEngine(error);
        crypto_compare->deleteLater();
    }, Qt::QueuedConnection);

    QUrlQuery params;
    crypto_compare->requestEndpoint("/data/all/coinlist", params);
}

void AppEngineLoader::downloadIcons() {
    Engine::LocalHash* hasher = new Engine::LocalHash;

    connect(hasher, &Engine::LocalHash::hashError, this, [this, hasher] (const QString& error) {
        emit errorEngine(error);
        hasher->deleteLater();
    }, Qt::QueuedConnection);

    connect(hasher, &Engine::LocalHash::allHashReady, this, [this, hasher] (const QHash<QString, QByteArray>& allBytes) {
        for (auto [url, bufIcon] : allBytes.asKeyValueRange()) {
            Engine::CoinIcon _icon = { url, bufIcon };
            for (auto& iCoin : m_data.infoAboutCoins)
                if (iCoin.coinIcon.url == url)
                    iCoin.coinIcon = _icon;
        }
        
        emit infoAboutIconsReady();
        hasher->deleteLater();
    }, Qt::QueuedConnection);

    QStringList lstUrl;
    for (const auto& iCoin : m_data.infoAboutCoins)
        lstUrl.append(iCoin.coinIcon.url);

    hasher->download(lstUrl);
}

void AppEngineLoader::processSymbols(const QJsonObject& data) {
    QJsonObject result = data["result"].toObject();
    QString category = result["category"].toString();
    QJsonArray list = result["list"].toArray();

    for (const auto& obj : list) {
        QJsonObject item = obj.toObject();

        Engine::TradingInfo data;
        data.market = category;
        data.sym = item["baseCoin"].toString();
        data.namePair = item["symbol"].toString();

        m_data.tradingPairs.append(data);
        m_data.loadedCoins.insert(data.sym);
    }
}

void AppEngineLoader::processInfoAboutCoins(const QJsonObject& data) {
    QJsonObject res_data = data["Data"].toObject();

    for (auto iCoin = res_data.constBegin(); iCoin != res_data.constEnd(); ++iCoin) {
        QString key = iCoin.key();

        if (!m_data.loadedCoins.contains(key))
            continue;

        QJsonObject objCoin = iCoin.value().toObject();

        Engine::InfoAboutCoin info;
        info.sym = objCoin["Symbol"].toString();
        info.fullName = objCoin["CoinName"].toString();
        info.description = objCoin["Description"].toString();

        Engine::CoinIcon icon;
        icon.url = Engine::CryptoCompare::baseUrl + objCoin["ImageUrl"].toString();
        info.coinIcon = icon;

        m_data.infoAboutCoins.append(info);
    }
}