#include "Parser/ParserMyWallet.hpp"
#include "LogHandler.hpp"
#include "Parser/BinanceParser.hpp"

#include <expected>
#include <QRegularExpression>

ParserMyWallet::StatMyWallet* ParserMyWallet::statMyWallet = nullptr;
QMap<QString, double> ParserMyWallet::amountEachAssets;
QMap<QString, QString> ParserMyWallet::rulesWallet;

void ParserMyWallet::updateWallet() {

    auto statA = statMyWallet->statEachAsset;
    auto& statW = statMyWallet->statWallet;

    double oldTotalPrice = statW.totalPrice;

    statW.totalPrice = 0;
    for (auto [name, amount] : amountEachAssets.asKeyValueRange())
        statW.totalPrice += statA[name].totalPrice;

    statW.difTotalPrice = statW.totalPrice - oldTotalPrice;
    statW.perDifPrice = statW.difTotalPrice / statW.totalPrice * 100;

    LogHandler::writeLog(QString("Wallet\n\tcur : %1\n\tnew : %2\n\tdif : %3 (%4)")
        .arg(QString::number(oldTotalPrice, 'f', 4))
        .arg(QString::number(statW.totalPrice, 'f', 4))
        .arg(QString::number(statW.difTotalPrice, 'f', 4))
        .arg(QString::number(statW.perDifPrice, 'f', 4) + "%").toUtf8());

}

void ParserMyWallet::updateAsset(const QString &coin, double price) {

    auto& statA = statMyWallet->statEachAsset;
    auto& statW = statMyWallet->statWallet;
    auto amount = amountEachAssets;

    double oldTotalPrice = (!statA.contains(coin)) ? amount[coin] * price : statA[coin].totalPrice;

    statA[coin].totalPrice = amount[coin] * price;
    statA[coin].difTotalPrice = statA[coin].totalPrice - oldTotalPrice;
    statA[coin].perDifPrice = statA[coin].difTotalPrice / statA[coin].totalPrice * 100;
    statA[coin].perDependence = statA[coin].totalPrice / statW.totalPrice * 100;

    LogHandler::writeLog(QString("Asset %1\n\tone : %2\n\tamount : %3\n\tcur : %4\n\tnew : %5\n\tdif : %6 (%7)\n\tdepend : %8")
        .arg(coin)
        .arg(QString::number(price, 'f', 4))
        .arg(QString::number(amount[coin], 'f', 4))
        .arg(QString::number(oldTotalPrice, 'f', 4))
        .arg(QString::number(statA[coin].totalPrice, 'f', 4))
        .arg(QString::number(statA[coin].difTotalPrice, 'f', 4))
        .arg(QString::number(statA[coin].perDifPrice, 'f', 4) + "%")
        .arg(QString::number(statA[coin].perDependence, 'f', 4) + "%").toUtf8());
}

void ParserMyWallet::updateAllAssets(const QString &coin, double price) {
    auto info = BinanceParser::getInfoAboutAllCoins();
    for (auto [name, _info] : info.asKeyValueRange())
        updateAsset(name, (name != coin) ? _info.value : price);
}

std::expected<double, QString> ParserMyWallet::parseRule(const QString& name, const QString& str) {
    QStringList args = str.split(' ');
    if (args.size() != 2)
        return std::unexpected("Incorrect rule set!");

    auto info = BinanceParser::getInfoAboutAllCoins();
    auto& amounts = amountEachAssets;

    if ( !info.contains(name) || !amounts.contains(name) )
        return std::unexpected("This asset does not exist!");

    RuleWallet rw;
    return rw(args[0], amounts[name], info[name].value, args[1]);
}

std::expected<double, QString> ParserMyWallet::parseAllRules() {
    double revenue = 0;
    for (auto [name, str] : rulesWallet.asKeyValueRange()) {
        auto exp = parseRule(name, str);
        if (exp.has_value())
            revenue += exp.value();
        else
            return std::unexpected(exp.error());
    }
    statMyWallet->statWallet.allRevenue += revenue;
    return revenue;
}

bool ParserMyWallet::RuleWallet::isAmount(const QString& str) {
    QRegularExpression regex(R"(^\d+(?:\.\d+)?$)");
    return regex.match(str).hasMatch();
}

bool ParserMyWallet::RuleWallet::isPercent(const QString& str) {
    QRegularExpression regex(R"(^\d+(?:\.\d+)?%$)");
    return regex.match(str).hasMatch();
}

std::expected<double, QString> ParserMyWallet::RuleWallet::buy_asset(double& amount, double priceRate, const QString& arg) {
    if (!isAmount(arg) && !isPercent(arg))
        return std::unexpected("The rule parameter is set incorrectly!");
    double cnt = (arg.back() != '%') ? arg.toDouble() : arg.left(arg.length()-1).toDouble()/100 * amount;
    double price = cnt * priceRate;
    amount += cnt;
    return -price;
}

std::expected<double, QString> ParserMyWallet::RuleWallet::sell_asset(double& amount, double priceRate, const QString& arg) {
    if (!isAmount(arg) && !isPercent(arg))
        return std::unexpected("The rule parameter is set incorrectly!");
    double cnt = (arg.back() != '%') ? arg.toDouble() : arg.left(arg.length()-1).toDouble()/100 * amount;
    double price = cnt * priceRate;
    amount -= cnt;
    return price;
}

std::expected<double, QString> ParserMyWallet::RuleWallet::stable_asset(double& amount, double priceRate, const QString& arg) {
    if (!isAmount(arg) && !isPercent(arg))
        return std::unexpected("The rule parameter is set incorrectly!");
    double cnt = (arg.back() != '%') ? arg.toDouble() : arg.left(arg.length()-1).toDouble()/100 * amount;
    double price = (amount - cnt) * priceRate;
    amount -= amount - cnt;
    return price;
}
