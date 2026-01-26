#include "CustomWindowDialogs/ViewerOrderBooksController.hpp"

void ViewerOrderBooksController::updateOrderBooks(const WebSocketParser::stOrderBooks& _orderBooks) {
    QStringList tokens = _orderBooks.namePair.split(':');
    if (m_dialog && tokens[0] == m_dialog->getCurrentStockMarkets() && tokens[1] == m_dialog->getCurrentPair())
        m_dialog->updateDisplay(_orderBooks.asks, _orderBooks.bids);
}

void ViewerOrderBooksController::onDialogCreated() {
    QStringList lstStockMarket = scanner->getListStockMarket();
    QStringList lstPairs = scanner->getScannerConfig().pairs;
    for (auto& pair : lstPairs)
        pair.replace("/", "");

    m_dialog->setStockMarkets(lstStockMarket);
    m_dialog->setPairs(lstPairs);

    QObject::connect(scanner, &Scanner::orderBooks, this, &ViewerOrderBooksController::updateOrderBooks, Qt::UniqueConnection);
}