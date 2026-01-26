#include "CustomWindowDialogs/TableController.hpp"

void TableController::updateTable(const WebSocketParser::stTicker& _ticker) {
    if (m_dialog) {
        QStringList tockens = _ticker.namePair.split(':');
        auto row = m_dialog->findIndexByVerticalHeaders(tockens[1]);
        auto col = m_dialog->findIndexByHorizontalHeaders(tockens[0]);
        m_dialog->fillTable(row, col, QString::number(_ticker.curPrice, 'f', 3));
    }
}

void TableController::onDialogCreated() {
    QStringList lstStockMarket = scanner->getListStockMarket();
    QStringList lstPairs = scanner->getScannerConfig().pairs;
    for (auto& pair : lstPairs)
        pair.replace("/", "");

    m_dialog->setSize(lstStockMarket.size(), lstPairs.size());
    m_dialog->setVerticalHeader(lstPairs);
    m_dialog->setHorizontalHeader(lstStockMarket);

    QObject::connect(scanner, &Scanner::ticker, this, &TableController::updateTable, Qt::UniqueConnection);
}