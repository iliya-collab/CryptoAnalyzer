#pragma one

#include "Configs/MyWalletConfig.hpp"
#include "Configs/ScannerConfig.hpp"

#include "LogHandler.hpp"

#include <functional>
#include <expected>

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
    // Правила кошелька
    static QMap<QString, QString> rulesWallet;
    // -------------------------------------------------

    // Класс, описывающий правила кошелька
    class RuleWallet {
    private:

        // Пара кляч - значение, где кляч - правило для этого актива, а значение - функция обработчик
        QMap<QString, std::function<std::expected<double, QString>(double&, double, const QString&)>> rules;

        // Функции обработчики правил
        // -------------------------------------------------
        // Покупает определенное кол-во данного актива, используя общую цену кошелька. В качестве результата возращает сумму, которая была потрачена
        std::expected<double, QString> buy_asset(double& amount, double priceRate, const QString& arg);
        // Продает определенное кол-во данного актива. В качестве результата возращает сумму, которая была выручена в результате продажи
        std::expected<double, QString> sell_asset(double& amount, double priceRate, const QString& arg);
        // Можен купить или продать определенное кол-во актива, в зависимости от исходного значения. Т.е. доводит кол-во данного актива до указанного значения
        std::expected<double, QString> stable_asset(double& amount, double priceRate, const QString& arg);
        // -------------------------------------------------

    public:

        RuleWallet() {
            rules["buy"] = [this](double& amount, double priceRate, const QString& arg) { return buy_asset(amount, priceRate, arg); };
            rules["sell"] = [this](double& amount, double priceRate, const QString& arg) { return sell_asset(amount, priceRate, arg); };
            rules["stable"] = [this](double& amount, double priceRate, const QString& arg) { return stable_asset(amount, priceRate, arg); };
        }

        std::expected<double, QString> operator() (const QString& rule, double& amount, double priceRate, const QString& arg) {
            return rules[rule](amount, priceRate, arg);
        }

        bool isAmount(const QString& str);
        bool isPercent(const QString& str);

    };

    std::expected<double, QString> parseRule(const QString& name, const QString& str);

public:

    ParserMyWallet() {
        auto curConfig = MyWalletConfig::instance().getConfig();

        statMyWallet = new StatMyWallet;
        statMyWallet->statWallet.allRevenue = 0;

        amountEachAssets = curConfig.asset;
        rulesWallet = curConfig.rules;

        LogHandler::setEnable(ScannerConfig::instance().getConfig().enableLogs);
        LogHandler::clearLog();
    }

    ~ParserMyWallet() {
        delete statMyWallet;
    }

    void updateAsset(const QString &coin, double price);
    void updateAllAssets(const QString &coin, double price);
    void updateWallet();

    std::expected<double, QString> parseAllRules();

    static StatMyWallet getStat() {
        return *statMyWallet;
    }

    static QMap<QString, double> getCurrentAmounts() {
        return amountEachAssets;
    }

    static double getAllRevenue() {
        return statMyWallet->statWallet.allRevenue;
    }

};
