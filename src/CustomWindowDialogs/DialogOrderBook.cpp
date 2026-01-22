#include "CustomWindowDialogs/DialogOrderBook.hpp"

DialogOrderBook::DialogOrderBook(QWidget* parent) : IDialog(parent) {
    setModal(false);
    setWindowTitle("Table");
    setMinimumSize(300, 200);
    resize(500, 400);

    setupUI();
}

void DialogOrderBook::setupUI() {
    subscribedCoins = new QComboBox(this);
    usedStockMarkets = new QComboBox(this);

    QHBoxLayout* row1 = new QHBoxLayout;
    row1->addWidget(usedStockMarkets);
    row1->addWidget(subscribedCoins);

    mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(row1);
    mainLayout->addStretch();
}