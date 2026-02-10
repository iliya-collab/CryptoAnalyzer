#include "Engine/AppEngine.hpp"

#include <QtConcurrent/QtConcurrent>

void AppEngine::init() {
    qDebug() << "--- Reading the platform configuration ---";
    if (!Settings::readAllConfig())
        qDebug() << Settings::getLastError();
}

void AppEngine::startSequentialDownload() {
    QtConcurrent::run([this]() {
        runSequentialLoading();
    });
}

void AppEngine::runSequentialLoading() {
    struct LoadingStep {
        QString name;
        std::function<void()> action;
    };

    QVector<LoadingStep> steps = {
        {"Loading SPOT pairs", [this]() { loadTradingPairsSync(Engine::TMarketData::SPOT); }},
        {"Loading LINEAR pairs", [this]() { loadTradingPairsSync(Engine::TMarketData::LINEAR); }},
        {"Loading INVERSE pairs", [this]() { loadTradingPairsSync(Engine::TMarketData::INVERSE); }},
        {"Loading OPTION pairs", [this]() { loadTradingPairsSync(Engine::TMarketData::OPTION); }},
        {"Loading coins info", [this]() { loadCoinsInfoSync(); }}
    };

    for (int i = 0; i < steps.size(); ++i) {
        QMetaObject::invokeMethod(this, [this, i, total = steps.size()]() {
            emit progressChanged(i + 1, total);
        });
        
        try {
            steps[i].action();
        } catch (const std::exception& e) {
            QMetaObject::invokeMethod(this, [this, msg = QString(e.what())]() {
                emit error(msg);
            });
            return;
        }
    }

    QMetaObject::invokeMethod(this, &AppEngine::finished);
}

void AppEngine::loadTradingPairsSync(Engine::TMarketData type) {
    /*QEventLoop loop;
    QTimer timer;
    
    connect(this, &AppEngine::loaded, &loop, &QEventLoop::quit);
    
    timer.setSingleShot(true);
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start(30000);
    
    getTradingPairs(type);
    
    loop.exec();
    
    if (!timer.isActive())
        throw std::runtime_error("Timeout loading trading pairs");*/

    QDeadlineTimer deadline(30000);
    bool success = false;
    
    auto conn = connect(this, &AppEngine::tradingPairsReady, [&success, type](Engine::TMarketData loadedType, const QStringList&) {
        if (loadedType == type)
            success = true;
    });
    
    auto errorConn = connect(this, &AppEngine::error, [&success](const QString&) {
        success = false;
    });
    
    getTradingPairs(type);
    
    while (!success && !deadline.hasExpired()) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        QThread::msleep(10);
    }
    
    disconnect(conn);
    disconnect(errorConn);
    
    if (!success)
        throw std::runtime_error("Timeout loading trading pairs");

}

void AppEngine::loadCoinsInfoSync() {
    /*QEventLoop loop;
    QTimer timer;
    
    connect(this, &AppEngine::loaded, &loop, &QEventLoop::quit);
    
    timer.setSingleShot(true);
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start(30000);
    
    getInfoAboutCoins();

    loop.exec();
    
    if (!timer.isActive())
        throw std::runtime_error("Timeout loading coins info");*/

    QDeadlineTimer deadline(30000);
    bool success = false;
    
    auto conn = connect(this, &AppEngine::infoAboutCoinsReady, [&success](const QHash<QString, Engine::InfoAboutCoin>&) {
        success = true;
    });
    
    auto errorConn = connect(this, &AppEngine::error, [&success](const QString&) {
        success = false;
    });
    
    getInfoAboutCoins();
    
    while (!success && !deadline.hasExpired()) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        QThread::msleep(10);
    }
    
    disconnect(conn);
    disconnect(errorConn);
    
    if (!success)
        throw std::runtime_error("Timeout loading coins info");
}

void AppEngine::setAPI(const Engine::API& api) {
    m_api = api;
}

void AppEngine::getTradingPairs(Engine::TMarketData market) {
    Engine::BybitRestAPI* bybit_api = new Engine::BybitRestAPI(m_api);

    bybit_api->moveToThread(this->thread());

    connect(bybit_api, &Engine::BybitRestAPI::dataReceived, this, [this, bybit_api, market] (const QJsonObject& data) {
        emit loaded();
        emit tradingPairsReady(market, processSymbols(data));
        bybit_api->deleteLater();
    });

    connect(bybit_api, &Engine::BybitRestAPI::errorOccurred, this, [this, bybit_api] (const QString& error) {
        emit loaded();
        qDebug() << error;
        bybit_api->deleteLater();
    });

    QString category;
    switch (market)
    {
    case Engine::TMarketData::SPOT:
        category = "spot";
        break;
    case Engine::TMarketData::LINEAR:
        category = "linear";
        break;
    case Engine::TMarketData::INVERSE:
        category = "inverse";
        break;
    case Engine::TMarketData::OPTION:
        category = "option";
        break;
    }

    QUrlQuery params;
    params.addQueryItem("category", category);

    QMetaObject::invokeMethod(bybit_api, [bybit_api, params]() {
        bybit_api->requestEndpoint("/v5/market/instruments-info", params, Engine::IRestAPI::TIMEOUT_REQUEST);
    });
    
    //bybit_api->requestEndpoint("/v5/market/instruments-info", params, Engine::IRestAPI::TIMEOUT_REQUEST);
    
}

void AppEngine::getInfoAboutCoins() {
    Engine::CryptoCompare* crypto_compare = new Engine::CryptoCompare;
    
    crypto_compare->moveToThread(this->thread());

    connect(crypto_compare, &Engine::CryptoCompare::dataReceived, this, [this, crypto_compare](const QJsonObject& data) {
        emit loaded();
        processInfoAboutCoins(data);
        crypto_compare->deleteLater();
    });

    connect(crypto_compare, &Engine::CryptoCompare::errorOccurred, this, [this, crypto_compare](const QString& error) {
        emit loaded();
        qDebug() << error;
        crypto_compare->deleteLater();
    });

    QUrlQuery params;
    QMetaObject::invokeMethod(crypto_compare, [crypto_compare, params]() {
        crypto_compare->requestEndpoint("/data/all/coinlist", params, Engine::IRestAPI::TIMEOUT_REQUEST);
    });
    //crypto_compare->requestEndpoint("/data/all/coinlist", params, Engine::IRestAPI::TIMEOUT_REQUEST);
}

void AppEngine::downloadAllIcons(const QHash<QString, QString>& icons) {
    Engine::LocalHash* hasher = new Engine::LocalHash(this);

    connect(hasher, &Engine::LocalHash::hashError, this, [this, hasher] (const QString& error) {
        qDebug() << error;
        hasher->deleteLater();
    });

    connect(hasher, &Engine::LocalHash::allHashReady, this, [this, hasher, icons] (const QHash<QString, QByteArray>& allBytes) {
        hasher->deleteLater();
        QHash<QString, QByteArray> lstIcons;

        for (auto [sym, url] : icons.asKeyValueRange()) {
            if (allBytes.contains(url))
                lstIcons.insert(sym, allBytes.value(url));
        }

        emit iconsReady(lstIcons);
    });

    hasher->download(icons.values());
}

QStringList AppEngine::processSymbols(const QJsonObject& data) {
    QJsonObject result = data["result"].toObject();
    QString category = result["category"].toString();
    QJsonArray list = result["list"].toArray();
    QStringList coins;

    m_usedCoins.clear();

    for (const auto& obj : list) {
        QJsonObject item = obj.toObject();
        coins.append(item["symbol"].toString());
        m_usedCoins.insert(item["baseCoin"].toString());
    }

    return coins;
}

void AppEngine::processInfoAboutCoins(const QJsonObject& data) {
    QHash<QString, Engine::InfoAboutCoin> coins;
    QHash<QString, QString> icons;

    QJsonObject res_data = data["Data"].toObject();

    for (auto iCoin = res_data.constBegin(); iCoin != res_data.constEnd(); ++iCoin) {
        QString key = iCoin.key();

        if (!m_usedCoins.contains(key))
            continue;

        QJsonObject objCoin = iCoin.value().toObject();

        Engine::InfoAboutCoin info;
        info.fullName = objCoin["CoinName"].toString();
        info.description = objCoin["Description"].toString();
        coins.insert(key, info);
        
        QString iconUrl = Engine::CryptoCompare::baseUrl + objCoin["ImageUrl"].toString();
        icons.insert(key, iconUrl);
    }

    emit infoAboutCoinsReady(coins);
    emit infoAboutIconsReady(icons);
}