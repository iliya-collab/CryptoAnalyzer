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
            qInfo().noquote() << steps[i].name;
            steps[i].action();
        } catch (const std::exception& e) {
            QMetaObject::invokeMethod(this, [this, msg = QString(e.what())]() {
                emit errorEngine(msg);
            });
            return;
        }
    }

    QMetaObject::invokeMethod(this, &AppEngine::finished);
}

void AppEngine::loadTradingPairsSync(Engine::TMarketData type) {
    QDeadlineTimer deadline(30000);
    bool success = false;
    
    auto conn = connect(this, &AppEngine::tradingPairsReady, [&success, type] (Engine::TMarketData loadedType, const QStringList&) {
        if (loadedType != type)
            return;

        success = true;
        
        QString category;
        switch (type)
        {
        case Engine::TMarketData::SPOT:
            category = "Spot";
            break;
        case Engine::TMarketData::LINEAR:
            category = "Linear";
            break;
        case Engine::TMarketData::INVERSE:
            category = "Inverse";
            break;
        case Engine::TMarketData::OPTION:
            category = "Option";
            break;
        }
        qInfo().noquote() << QString("%1 pairs ready").arg(category);
    });
    
    auto errorConn = connect(this, &AppEngine::errorEngine, [&success] (const QString&) {
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
    QDeadlineTimer deadline(30000);
    bool success = false;
    
    auto conn = connect(this, &AppEngine::infoAboutCoinsReady, [&success](const QHash<QString, Engine::InfoAboutCoin>&) {
        success = true;
    });
    
    auto errorConn = connect(this, &AppEngine::errorEngine, [&success](const QString&) {
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

    connect(bybit_api, &Engine::BybitRestAPI::dataReceived, this, [this, bybit_api, market] (const QJsonObject& data) {
        emit tradingPairsReady(market, processSymbols(data));
        bybit_api->deleteLater();
    }, Qt::QueuedConnection);

    connect(bybit_api, &Engine::BybitRestAPI::errorOccurred, this, [this, bybit_api] (const QString& error) {
        emit errorEngine(error);
        bybit_api->deleteLater();
    }, Qt::QueuedConnection);

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
    bybit_api->requestEndpoint("/v5/market/instruments-info", params, Engine::IRestAPI::TIMEOUT_REQUEST);
    
}

void AppEngine::getInfoAboutCoins() {
    Engine::CryptoCompare* crypto_compare = new Engine::CryptoCompare;
    
    connect(crypto_compare, &Engine::CryptoCompare::dataReceived, this, [this, crypto_compare](const QJsonObject& data) {
        processInfoAboutCoins(data);
        crypto_compare->deleteLater();
    }, Qt::QueuedConnection);

    connect(crypto_compare, &Engine::CryptoCompare::errorOccurred, this, [this, crypto_compare](const QString& error) {
        emit errorEngine(error);
        crypto_compare->deleteLater();
    }, Qt::QueuedConnection);

    QUrlQuery params;
    crypto_compare->requestEndpoint("/data/all/coinlist", params, Engine::IRestAPI::TIMEOUT_REQUEST);
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