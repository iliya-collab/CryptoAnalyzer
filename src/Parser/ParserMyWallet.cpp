#include "Parser/ParserMyWallet.hpp"
#include "Parser/BinanceParser.hpp"

#include <expected>
#include <QRegularExpression>

ParserMyWallet::StatMyWallet* ParserMyWallet::statMyWallet = nullptr;
QMap<QString, double> ParserMyWallet::amountEachAssets;

void ParserMyWallet::updateWallet() {

    auto statA = statMyWallet->statEachAsset;
    auto& statW = statMyWallet->statWallet;

    double oldTotalPrice = statW.totalPrice;

    statW.totalPrice = 0;
    for (auto [name, amount] : amountEachAssets.asKeyValueRange())
        statW.totalPrice += statA[name].totalPrice;

    statW.difTotalPrice = statW.totalPrice - oldTotalPrice;
    statW.perDifPrice = statW.difTotalPrice / statW.totalPrice * 100;

    qDebug() << QString("Wallet\n\tcur : %1\n\tnew : %2\n\tdif : %3 (%4)")
        .arg(QString::number(oldTotalPrice, 'f', 4))
        .arg(QString::number(statW.totalPrice, 'f', 4))
        .arg(QString::number(statW.difTotalPrice, 'f', 4))
        .arg(QString::number(statW.perDifPrice, 'f', 4) + "%").toUtf8();

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

    qDebug() << QString("Asset %1\n\tone : %2\n\tamount : %3\n\tcur : %4\n\tnew : %5\n\tdif : %6 (%7)\n\tdepend : %8")
        .arg(coin)
        .arg(QString::number(price, 'f', 4))
        .arg(QString::number(amount[coin], 'f', 4))
        .arg(QString::number(oldTotalPrice, 'f', 4))
        .arg(QString::number(statA[coin].totalPrice, 'f', 4))
        .arg(QString::number(statA[coin].difTotalPrice, 'f', 4))
        .arg(QString::number(statA[coin].perDifPrice, 'f', 4) + "%")
        .arg(QString::number(statA[coin].perDependence, 'f', 4) + "%").toUtf8();
}

void ParserMyWallet::updateAllAssets(const QString &coin, double price) {
    auto info = ptrParser->getInfoAboutAllCoins();
    for (auto [name, _info] : info.asKeyValueRange())
        updateAsset(name, (name != coin) ? _info.stPrice.curPrice : price);
}