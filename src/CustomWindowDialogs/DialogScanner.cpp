#include "CustomWindowDialogs/DialogScanner.hpp"

DialogScanner::DialogScanner(QWidget* parent) : CustomQDialog(parent) {
    resize(600, 400);
    setWindowTitle("Scanner");

    setupUI();
    setupMenu();
    connectionSignals();

    TController = std::make_unique<TableController>(this);
    _scan = std::make_unique<Scanner>(this);
}


void DialogScanner::setupUI() {
    QHBoxLayout* row1 = new QHBoxLayout;

    comboStockMarket = new QComboBox(this);
    comboMarket = new QComboBox(this);
    comboChannel = new QComboBox(this);

    btnAdd = new QPushButton(this);
    btnAdd->setText("Add");
    btnAdd->setFocusPolicy(Qt::NoFocus);

    comboStockMarket->addItems({
        "Binance",
        "Bybit",
        "OKX"
    });
    comboMarket->addItems({
        "spot",
        "futures",
        "test"
    });
    comboChannel->addItems({
        "ticker",
        "books5",
        "books10",
        "books20"
    });

    row1->addWidget(comboStockMarket);
    row1->addWidget(comboMarket);
    row1->addWidget(comboChannel);
    row1->addStretch();
    row1->addWidget(btnAdd);

    QHBoxLayout* row2 = new QHBoxLayout;
    btnOK = new QPushButton(this);
    btnOK->setText("OK");
    btnOK->setFocusPolicy(Qt::NoFocus);

    row2->addWidget(btnOK);
    row2->addStretch();
    row2->addStretch();

    layout = new QVBoxLayout(this);
    layout->addLayout(row1);
    layout->addStretch();
    layout->addLayout(row2);
}

void DialogScanner::setupMenu() {
    QMenuBar* menuBar = new QMenuBar(this);
    QMenu* menuWindow = menuBar->addMenu("Windows");
    actionTabel = new QAction("Table", this);
    menuWindow->addAction(actionTabel);
    actionGraph = new QAction("Graph", this);
    menuWindow->addAction(actionGraph);
    layout->setMenuBar(menuBar);
}

void DialogScanner::connectionSignals() {
    connect(btnOK, &QPushButton::clicked, this, &DialogScanner::onClickedButtonOk);
    connect(btnAdd, &QPushButton::clicked, this, &DialogScanner::onClickedButtonAdd);
    connect(actionTabel, &QAction::triggered, this, &DialogScanner::onDialogTableActivated);
    connect(actionGraph, &QAction::triggered, this, &DialogScanner::onDialogGraphActivated);
}

void DialogScanner::onClickedButtonOk() {
    //qDebug() << "DialogScanner::onClickedButtonOk";
    _scan->start();
}

void DialogScanner::onClickedButtonAdd() {
    //qDebug() << "DialogScanner::onClickedButtonAdd";
    _scan->addStockMarket(comboStockMarket->currentText(), comboMarket->currentText(), comboChannel->currentText());
}

void DialogScanner::onDialogTableActivated() {
    //qDebug() << "DialogScanner::onDialogTableActivated";
    QStringList lstStockMarket = _scan->getListStockMarket();
    QStringList lstPairs = _scan->getScannerConfig().pairs;
    for (auto& pair : lstPairs)
        pair.replace("/", "");
    TController->showTable(lstStockMarket.size(), lstPairs.size(), lstPairs, lstStockMarket);
    connect(_scan.get(), &Scanner::update_coin, this, &DialogScanner::updateTable, Qt::UniqueConnection);
}

void DialogScanner::onDialogGraphActivated() {
    //qDebug() << "DialogScanner::onDialogGraphActivated";
    connect(_scan.get(), &Scanner::update_coin, this, &DialogScanner::updateGraph, Qt::UniqueConnection);
}

void DialogScanner::updateTable(const QString &symbol, const WebSocketParser::stInfoCoin& _info) {
    TController->updateTable(symbol, _info.stPrice.curPrice);
}

void DialogScanner::updateGraph(const QString &symbol, const WebSocketParser::stInfoCoin& _info) {

}
