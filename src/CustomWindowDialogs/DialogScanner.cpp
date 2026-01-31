#include "CustomWindowDialogs/DialogScanner.hpp"

DialogScanner::DialogScanner(QWidget* parent) : IDialog(parent) {
    setModal(false);
    resize(600, 400);
    setWindowTitle("Scanner");

    setupUI();
    setupMenu();
    connectionSignals();

    m_scanner = std::make_unique<Scanner>();
    m_crtl_table = std::make_unique<TableController>(m_scanner.get(), this);
    m_crtl_ord_books = std::make_unique<ViewerOrderBooksController>(m_scanner.get(), this);
}


void DialogScanner::setupUI() {
    QHBoxLayout* row1 = new QHBoxLayout;

    comboStockMarket = new QComboBox(this);

    treeViewer = new TreeViewWidget(this);

    btnAdd = new QPushButton(this);
    btnAdd->setText("Add");
    btnAdd->setFocusPolicy(Qt::NoFocus);
    btnDel = new QPushButton(this);
    btnDel->setText("Del");
    btnDel->setFocusPolicy(Qt::NoFocus);

    comboStockMarket->addItems({
        "Binance/spot",
        "Binance/futures",
        "Bybit/spot",
        "Bybit/futures",
        "OKX/spot",
        "OKX/futures"
    });

    row1->addWidget(comboStockMarket);
    row1->addStretch();
    row1->addWidget(btnAdd);
    row1->addWidget(btnDel);

    QHBoxLayout* row2 = new QHBoxLayout;
    btnStart = new QPushButton(this);
    btnStart->setText("Start");
    btnStart->setFocusPolicy(Qt::NoFocus);
    btnStop = new QPushButton(this);
    btnStop->setText("Stop");
    btnStop->setFocusPolicy(Qt::NoFocus);

    row2->addWidget(btnStart);
    row2->addWidget(btnStop);
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
    
    QMenu* channelsMenu = menuBar->addMenu("Channels");
    channelsMenu->setMinimumWidth(150);

    QMenu* menuTicker = channelsMenu->addMenu("Ticker");
    tickerChannelCheck = new QCheckBox("Enable", this);
    QWidgetAction* tickerAction = new QWidgetAction(this);
    tickerAction->setDefaultWidget(tickerChannelCheck);    
    menuTicker->addAction(tickerAction);

    QMenu* menuBooks = channelsMenu->addMenu("Order Books");
    QWidgetAction* booksAction = new QWidgetAction(this);
    booksAction->setDefaultWidget(createOrderBooksWidget());    
    menuBooks->addAction(booksAction);

    QMenu* menuAvailable = menuBar->addMenu("Available");
    actionTicker = new QAction("Ticker", this);
    menuAvailable->addAction(actionTicker);
    actionOrderBooks = new QAction("Order books", this);
    menuAvailable->addAction(actionOrderBooks);
}

void DialogScanner::connectionSignals() {
    connect(comboStockMarket, &QComboBox::currentTextChanged, this, &DialogScanner::onCurrentTextChanged);

    connect(booksGroup, &QButtonGroup::idClicked, this, [this](int idBut) {
        QString stockMarket = comboStockMarket->currentText();

        if (!RegisterParsers::instanse().hasRegistered(stockMarket))
            return;

        RegisterParsers::instanse().deleteParser(stockMarket, "books5");
        RegisterParsers::instanse().deleteParser(stockMarket, "books10");
        RegisterParsers::instanse().deleteParser(stockMarket, "books20");

        QRadioButton* radio = static_cast<QRadioButton*>(booksGroup->button(idBut));
        if (radio && radio->isChecked()) {
            switch (idBut)
            {
            case IdButtons::IdBooks5Radio:
                RegisterParsers::instanse().registerParser(stockMarket, "books5");
                break;
            case IdButtons::IdBooks10Radio:
                RegisterParsers::instanse().registerParser(stockMarket, "books10");
                break;
            case IdButtons::IdBooks20Radio:
                RegisterParsers::instanse().registerParser(stockMarket, "books20");
                break;
            default:
                break;
            }
        }

        m_scanner->addChannels(stockMarket, RegisterParsers::instanse().getRegisteredChannels(stockMarket));
    });
    
    connect(tickerChannelCheck, &QCheckBox::checkStateChanged, [this](Qt::CheckState state) {
        QString stockMarket = comboStockMarket->currentText();

        if (!RegisterParsers::instanse().hasRegistered(stockMarket))
            return;

        if (state == Qt::Checked)
            RegisterParsers::instanse().registerParser(stockMarket, "ticker");
        else if (state == Qt::Unchecked)
            RegisterParsers::instanse().deleteParser(stockMarket, "ticker");

        m_scanner->addChannels(stockMarket, RegisterParsers::instanse().getRegisteredChannels(stockMarket));
    });

    connect(btnStart, &QPushButton::clicked, this, [this]() { m_scanner->start(); });
    connect(btnStop, &QPushButton::clicked, this, [this]() { m_scanner->stop(); });
    connect(btnAdd, &QPushButton::clicked, this, &DialogScanner::onClickedButtonAdd);
    connect(btnDel, &QPushButton::clicked, this, &DialogScanner::onClickedButtonDel);

    connect(actionTabel, &QAction::triggered, this, [this]() { 
        m_crtl_table->create();
        m_crtl_table->show();
    });
    connect(actionOrderBooks, &QAction::triggered, this, [this]() { 
        m_crtl_ord_books->create();
        m_crtl_ord_books->show();
    });
}

void DialogScanner::onCurrentTextChanged(const QString& text) {

    tickerChannelCheck->setChecked(false);
    booksNoneRadio->setChecked(true);

    if (!RegisterParsers::instanse().hasRegistered(text))
        return;

    if (RegisterParsers::instanse().hasRegistered(text, "ticker"))
        tickerChannelCheck->setChecked(true);
        
    if (RegisterParsers::instanse().hasRegistered(text, "books20"))
        books20Radio->setChecked(true);
    else if (RegisterParsers::instanse().hasRegistered(text, "books10"))
        books10Radio->setChecked(true);
    else if (RegisterParsers::instanse().hasRegistered(text, "books5"))
        books5Radio->setChecked(true);

}

QWidget* DialogScanner::createOrderBooksWidget() {
    QWidget* widget = new QWidget(this);
    QVBoxLayout* groupLayout = new QVBoxLayout(widget);

    booksGroup = new QButtonGroup(widget);

    booksNoneRadio = new QRadioButton("None", widget);
    books5Radio = new QRadioButton("5 levels", widget);
    books10Radio = new QRadioButton("10 levels", widget);
    books20Radio = new QRadioButton("20 levels", widget);

    booksGroup->addButton(booksNoneRadio, IdButtons::IdBooksNoneRadio);
    booksGroup->addButton(books5Radio, IdButtons::IdBooks5Radio);
    booksGroup->addButton(books10Radio, IdButtons::IdBooks10Radio);
    booksGroup->addButton(books20Radio, IdButtons::IdBooks20Radio);

    booksNoneRadio->setChecked(true);

    groupLayout->addWidget(booksNoneRadio);
    groupLayout->addWidget(books5Radio);
    groupLayout->addWidget(books10Radio);
    groupLayout->addWidget(books20Radio);

    return widget;
}

void DialogScanner::onClickedButtonAdd() {
    QString stockMarket = comboStockMarket->currentText();
    m_scanner->addStockMarket(stockMarket);
    treeViewer->addItem(stockMarket.replace('/', '-'));
}

void DialogScanner::onClickedButtonDel() {
    QString stockMarket = comboStockMarket->currentText();
    m_scanner->delStockMarket(stockMarket);
    treeViewer->removeItem(stockMarket.replace('/', '-'));
}

Scanner* DialogScanner::scanner() {
    return m_scanner.get();
}