#include "CustomWindowDialogs/DialogScanner.hpp"

DialogScanner::DialogScanner(QWidget* parent) : IDialog(parent) {
    resize(600, 400);
    setWindowTitle("Scanner");

    setupUI();
    setupMenu();
    connectionSignals();

    _scan = std::make_unique<Scanner>();
    m_crtl_table = std::make_unique<TableController>(_scan.get(), this);
    m_crtl_ord_books = std::make_unique<ViewerOrderBooksController>(_scan.get(), this);
}


void DialogScanner::setupUI() {
    QHBoxLayout* row1 = new QHBoxLayout;

    comboStockMarket = new QComboBox(this);
    comboMarket = new QComboBox(this);
    comboChannel = new QComboBox(this);
    treeViewer = new TreeViewWidget(this);

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

    mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(row1);
    mainLayout->addWidget(treeViewer);
    mainLayout->addLayout(row2);
}

void DialogScanner::setupMenu() {
    QMenuBar* menuBar = new QMenuBar(this);
    mainLayout->setMenuBar(menuBar);

    QMenu* menuWindow = menuBar->addMenu("Windows");
    actionTabel = new QAction("Table", this);
    menuWindow->addAction(actionTabel);
    actionGraph = new QAction("Graph", this);
    menuWindow->addAction(actionGraph);
    
    QMenu* menuChannels = menuBar->addMenu("Channels");
    actionTicker = new QAction("Ticker", this);
    menuChannels->addAction(actionTicker);
    actionOrderBooks = new QAction("Order books", this);
    menuChannels->addAction(actionOrderBooks);
}

void DialogScanner::connectionSignals() {
    connect(btnOK, &QPushButton::clicked, this, &DialogScanner::onClickedButtonOk);
    connect(btnAdd, &QPushButton::clicked, this, &DialogScanner::onClickedButtonAdd);
    connect(btnDel, &QPushButton::clicked, this, &DialogScanner::onClickedButtonDel);
    connect(actionTabel, &QAction::triggered, this, &DialogScanner::onDialogTableActivated);
    connect(actionGraph, &QAction::triggered, this, &DialogScanner::onDialogGraphActivated);
    connect(actionOrderBooks, &QAction::triggered, this, &DialogScanner::onDialogOrderBookActivated);
}

void DialogScanner::onClickedButtonOk() {
    _scan->start();
}

void DialogScanner::onClickedButtonAdd() {
    _scan->addStockMarket(comboStockMarket->currentText(), comboMarket->currentText(), comboChannel->currentText());
    treeViewer->addItem(QString("%1/%2")
        .arg(comboStockMarket->currentText())
        .arg(comboMarket->currentText())
    );
}

void DialogScanner::onClickedButtonDel() {
    _scan->delStockMarket(comboStockMarket->currentText(), comboMarket->currentText());
    treeViewer->removeItem(QString("%1/%2")
        .arg(comboStockMarket->currentText())
        .arg(comboMarket->currentText())
    );
}

void DialogScanner::onDialogTableActivated() {
    m_crtl_table->create();
    m_crtl_table->show();
}

void DialogScanner::onDialogGraphActivated() {
}

void DialogScanner::onDialogOrderBookActivated() {
    m_crtl_ord_books->create();
    m_crtl_ord_books->show();
}

Scanner* DialogScanner::scanner() {
    return _scan.get();
}