#pragma one

#include "Configs/MyWalletConfig.hpp"
#include "Configs/ScannerConfig.hpp"
#include "Parser/WebSocketParser.hpp"

class ParserMyWallet {
private:

    struct statAsset {
        // общая цена
        double totalPrice;
        // изменениe в цене (новая - старая)
        double difTotalPrice;
        // изменение в процентах (difTotalPrice / totalPrice * 100)
        double perDifPrice;
        // зависимость от конкретного актива в процентах
        double perDependence;
        // Общий доход от кошелька
        double allRevenue;
    };

    struct StatMyWallet {
        // Статистика по всему кошельку
        statAsset statWallet;
        // Общая статистика по каждому из активов
        QMap<QString, statAsset> statEachAsset;
    };

    // Статистика кошелька
    static StatMyWallet* statMyWallet;

    // Параметры беруться из MyWalletConfig
    // -------------------------------------------------
    // Кол-во по каждому из активов
    static QMap<QString, double> amountEachAssets;
    // -------------------------------------------------

    WebSocketParser* ptrParser;

public:

    ParserMyWallet() {
        auto curConfig = MyWalletConfig::instance().getConfig();

        statMyWallet = new StatMyWallet;
        statMyWallet->statWallet.allRevenue = 0;

        amountEachAssets = curConfig.asset;
    }

    ~ParserMyWallet() {
        delete statMyWallet;
    }

    void updateAsset(const QString &coin, double price);
    void updateAllAssets(const QString &coin, double price);
    void updateWallet();

    static StatMyWallet getStat() {
        return *statMyWallet;
    }

    static QMap<QString, double> getCurrentAmounts() {
        return amountEachAssets;
    }

    static double getAllRevenue() {
        return statMyWallet->statWallet.allRevenue;
    }

    void setWebSocketRarser(WebSocketParser* ptr) {
        ptrParser = ptr;
    }

};
