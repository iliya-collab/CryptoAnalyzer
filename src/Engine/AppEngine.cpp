#include "Engine/AppEngine.hpp"

void AppEngine::init() {
    qDebug() << "--- Reading the platform configuration ---";
    if (!Settings::readAllConfig())
        qDebug() << Settings::getLastError();
}

void AppEngine::setLaunchParams(const EngineLaunchParams& launch) {
    if (m_webSocket)
        m_webSocket.reset();
    m_launch = launch;
    m_webSocket = std::unique_ptr<Engine::WebSocket>(
        Engine::WebSocketBuilder::create(m_launch.m_stockMarket , m_launch.m_market)->get()
    );
}

void AppEngine::setAPI(const Engine::API& api) {
    m_api = api;
}

void AppEngine::getSpotTradingPairs() {
    Engine::BybitRestAPI* bybit_api = new Engine::BybitRestAPI(m_api, this);

    connect(bybit_api, &Engine::BybitRestAPI::dataReceived, this, [this, bybit_api] (const QJsonObject& data) {
        processSymbols(data);
        bybit_api->deleteLater();
    });

    connect(bybit_api, &Engine::BybitRestAPI::errorOccurred, this, [this, bybit_api] (const QString& error) {
        qDebug() << error;
        bybit_api->deleteLater();
    });

    QUrlQuery params;
    params.addQueryItem("category", "spot");
    bybit_api->requestEndpoint("/v5/market/instruments-info", params, 10000);
}

void AppEngine::getFuturesContracts() {
    Engine::BybitRestAPI* bybit_api = new Engine::BybitRestAPI(m_api, this);
    
    connect(bybit_api, &Engine::BybitRestAPI::dataReceived, this, [this, bybit_api] (const QJsonObject& data) {
        processSymbols(data);
        bybit_api->deleteLater();
    });

    connect(bybit_api, &Engine::BybitRestAPI::errorOccurred, this, [this, bybit_api] (const QString& error) {
        qDebug() << error;
        bybit_api->deleteLater();
    });

    QUrlQuery params;
    params.addQueryItem("category", "linear");
    bybit_api->requestEndpoint("/v5/market/instruments-info", params, 10000);
}

void AppEngine::getInfoAboutCoins() {
    Engine::CryptoCompare* crypto_compare = new Engine::CryptoCompare(this);
    
    connect(crypto_compare, &Engine::CryptoCompare::dataReceived, this, [this, crypto_compare](const QJsonObject& data) {
        processInfoAboutCoins(data);
        crypto_compare->deleteLater();
    });

    connect(crypto_compare, &Engine::CryptoCompare::errorOccurred, this, [this, crypto_compare](const QString& error) {
        qDebug() << error;
        crypto_compare->deleteLater();
    });

    QUrlQuery params;
    crypto_compare->requestEndpoint("/data/all/coinlist", params, 10000);
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
        // TODO
        emit iconsReady(lstIcons);
    });

    hasher->download(icons.values());
}

void AppEngine::processSymbols(const QJsonObject& data) {
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

    if (category == "spot")
        emit spotReady(coins);
    else if (category == "linear")
        emit futuresReady(coins);
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