#include "CustomWindowDialogs/DialogScanner.hpp"

DialogScanner::DialogScanner(QWidget* parent) : CustomQDialog(parent) {
    resize(600, 400);
    setWindowTitle("Scanner");

    setupUI();
    setupMenu();
    connectionSignals();

    TController = std::make_unique<TableController>(this);
    _scan = std::make_unique<Scanner>();
}


void DialogScanner::setupUI() {
    QHBoxLayout* row1 = new QHBoxLayout;

    comboStockMarket = new QComboBox(this);
    comboMarket = new QComboBox(this);
    comboChannel = new QComboBox(this);
    treeViewer = new TreeViewWidget(this);

    /*treeViewer->addItems("Фрукты", {"Яблоки", "Бананы", "Апельсины"});
    treeViewer->addItems("Овощи", {"Морковь", "Помидоры", "Огурцы"});
    treeViewer->addItems("Напитки", {"Сок", "Вода", "Чай"});
    treeViewer->addItem("Фрукты/Яблоки/Красные");
    treeViewer->removeItem("Напитки");*/

    btnAdd = new QPushButton(this);
    btnAdd->setText("Add");
    btnAdd->setFocusPolicy(Qt::NoFocus);
    btnDel = new QPushButton(this);
    btnDel->setText("Del");
    btnDel->setFocusPolicy(Qt::NoFocus);

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
    row1->addWidget(btnDel);

    QHBoxLayout* row2 = new QHBoxLayout;
    btnOK = new QPushButton(this);
    btnOK->setText("OK");
    btnOK->setFocusPolicy(Qt::NoFocus);

    row2->addWidget(btnOK);
    row2->addStretch();
    row2->addStretch();

    layout = new QVBoxLayout(this);
    layout->addLayout(row1);
    layout->addWidget(treeViewer);
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
    connect(btnDel, &QPushButton::clicked, this, &DialogScanner::onClickedButtonDel);

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
    treeViewer->addItem(QString("%1/%2/%3")
        .arg(comboStockMarket->currentText())
        .arg(comboMarket->currentText())
        .arg(comboChannel->currentText())
    );
}

void DialogScanner::onClickedButtonDel() {
    //qDebug() << "DialogScanner::onClickedButtonDel";
    _scan->delStockMarket(comboStockMarket->currentText(), comboMarket->currentText());
    treeViewer->removeItem(QString("%1/%2")
        .arg(comboStockMarket->currentText())
        .arg(comboMarket->currentText())
    );
}

void DialogScanner::onDialogTableActivated() {
    //qDebug() << "DialogScanner::onDialogTableActivated";
    QStringList lstStockMarket = _scan->getListStockMarket();
    QStringList lstPairs = _scan->getScannerConfig().pairs;
    for (auto& pair : lstPairs)
        pair.replace("/", "");
    TController->showTable(lstStockMarket.size(), lstPairs.size(), lstPairs, lstStockMarket);
    connect(_scan.get(), &Scanner::ticker, this, &DialogScanner::updateTable, Qt::UniqueConnection);
}

void DialogScanner::onDialogGraphActivated() {
    //qDebug() << "DialogScanner::onDialogGraphActivated";
    connect(_scan.get(), &Scanner::ticker, this, &DialogScanner::updateGraph, Qt::UniqueConnection);
}

void DialogScanner::updateTable(const WebSocketParser::stTicker& _ticker) {
    TController->updateTable(_ticker.namePair, _ticker.curPrice);
}

void DialogScanner::updateGraph(const WebSocketParser::stTicker& _ticker) {

}

Scanner* DialogScanner::scanner() {
    return _scan.get();
}