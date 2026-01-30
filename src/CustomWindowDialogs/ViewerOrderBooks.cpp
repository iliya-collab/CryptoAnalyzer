#include "CustomWindowDialogs/ViewerOrderBooks.hpp"
#include "Parser/RegisterParsers.hpp"

ViewerOrderBooks::ViewerOrderBooks(QWidget* parent) : IDialog(parent) {
    setModal(false);
    setWindowTitle("Order Books");
    setMinimumSize(300, 200);
    resize(500, 400);
    
    setupUI();
    setupConnection();
   
    updateChannelAvailability(comboStockMarkets->currentText());
}

void ViewerOrderBooks::setupUI() {
    comboSubscribedCoins = new QComboBox(this);
    comboStockMarkets = new QComboBox(this);
    stackWidgets = new QStackedWidget(this);

    QHBoxLayout* row1 = new QHBoxLayout;
    row1->addWidget(comboStockMarkets);
    row1->addWidget(comboSubscribedCoins);

    mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(row1);
    mainLayout->addWidget(stackWidgets);

    setupOrderBooksTable();
    setupMessage();
}

void ViewerOrderBooks::setupConnection() {
    connect(comboStockMarkets, &QComboBox::currentTextChanged, this, &ViewerOrderBooks::onComboTextChanged);
}

void ViewerOrderBooks::updateChannelAvailability(const QString& market) {
    m_hasChannel =  RegisterParsers::instanse().hasRegistered(market, "books5") || 
                    RegisterParsers::instanse().hasRegistered(market, "books10")|| 
                    RegisterParsers::instanse().hasRegistered(market, "books20");
    
    if (m_hasChannel)
        stackWidgets->setCurrentIndex(0);
    else
        stackWidgets->setCurrentIndex(1);
}

void ViewerOrderBooks::onComboTextChanged(const QString &text) {
    updateChannelAvailability(text);
}

void ViewerOrderBooks::setStockMarkets(const QStringList& lst) {
    comboStockMarkets->addItems(lst);
}

void ViewerOrderBooks::setPairs(const QStringList& lst) {
    comboSubscribedCoins->addItems(lst);
}

QString ViewerOrderBooks::getCurrentStockMarkets() {
    return comboStockMarkets->currentText();
}

QString ViewerOrderBooks::getCurrentPair() {
    return comboSubscribedCoins->currentText();
}

void ViewerOrderBooks::setupOrderBooksTable() {
    QWidget* tableContainer = new QWidget(this);
    QVBoxLayout* containerLayout = new QVBoxLayout(tableContainer);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(5);

    lblSpread = new QLabel(tableContainer);

    lblSpread->setStyleSheet(
        "border-radius: 3px;"
        "qproperty-alignment: AlignCenter;"
        "qproperty-wordWrap: true;"
    );

    QFont spreadFont = lblSpread->font();
    spreadFont.setPointSize(9);
    spreadFont.setBold(true);
    lblSpread->setFont(spreadFont);

    orderBooksTable = new QTableWidget(tableContainer);
    orderBooksTable->setColumnCount(2);
    orderBooksTable->setHorizontalHeaderLabels({"Price", "Size"});
    
    orderBooksTable->horizontalHeader()->setStretchLastSection(true);
    orderBooksTable->verticalHeader()->setVisible(false);
    orderBooksTable->setShowGrid(false);
    orderBooksTable->setAlternatingRowColors(true);
    
    orderBooksTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    orderBooksTable->setColumnWidth(0, 100);
    orderBooksTable->setColumnWidth(1, 150);

    orderBooksTable->setStyleSheet(
        "QTableWidget {"
        "gridline-color: #ddd;"
        "}"
        "QTableWidget::item {"
        "border-right: 1px solid #ccc;"
        "padding-right: 5px;"
        "}"
        "QTableWidget::item:last {"
        "border-right: none;"
        "}"
    );
    
    containerLayout->addWidget(lblSpread);
    containerLayout->addWidget(orderBooksTable);
    
    stackWidgets->addWidget(tableContainer);
}

void ViewerOrderBooks::setupMessage() {
    lblMessage = new QLabel(this);

    lblMessage->setStyleSheet(
        "border-radius: 3px;"
        "qproperty-alignment: AlignCenter;"
        "qproperty-wordWrap: true;"
    );

    QFont spreadFont = lblMessage->font();
    spreadFont.setPointSize(9);
    spreadFont.setBold(true);
    lblMessage->setFont(spreadFont);

    lblMessage->setText("You need to use the books5, books10, or books20 channels");

    stackWidgets->addWidget(lblMessage);
}

void ViewerOrderBooks::updateSpread(double bestAsk, double bestBid) {
    double spread = bestAsk - bestBid;
    QString spreadText = QString("ASK: %1  |  BID: %2  |  SPREAD: %3")
                        .arg(bestAsk, 0, 'f', 2)
                        .arg(bestBid, 0, 'f', 2)
                        .arg(spread, 0, 'f', 2);
    
    lblSpread->setText(spreadText);
}

void ViewerOrderBooks::updateAsks(const QList<WebSocketParser::Ask>& asks, double mxSize) {
    for (int i = 0; i < asks.size(); ++i) {
        int row = orderBooksTable->rowCount();
        orderBooksTable->insertRow(row);
        
        QTableWidgetItem* priceItem = new QTableWidgetItem(QString::number(asks[i].askPrice, 'f', 2));
        priceItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        priceItem->setForeground(Qt::red);
        
        if (i == 0)
            priceItem->setData(Qt::UserRole, "top_ask");
        
        orderBooksTable->setItem(row, 0, priceItem);
        
        QProgressBar* bar = new QProgressBar;
        double percentage = (asks[i].askSize / mxSize) * 100.0;
        bar->setRange(0, 100);
        bar->setValue(percentage);
        bar->setTextVisible(true);
        bar->setFormat(QString::number(asks[i].askSize, 'f', 4));
        bar->setStyleSheet(
            "QProgressBar {"
            "border: 1px solid #ffcccc;"
            "border-radius: 3px;"
            "text-align: center;"
            "}"
            "QProgressBar::chunk {"
            "background-color: #ff6b6b;"
            "border-radius: 2px;"
            "}"
        );
        orderBooksTable->setCellWidget(row, 1, bar);
    }
}

void ViewerOrderBooks::updateBids(const QList<WebSocketParser::Bid>& bids, double mxSize) {
    for (int i = 0; i < bids.size(); ++i) {
        int row = orderBooksTable->rowCount();
        orderBooksTable->insertRow(row);
        
        QTableWidgetItem* priceItem = new QTableWidgetItem(QString::number(bids[i].bidPrice, 'f', 2));
        priceItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        priceItem->setForeground(QColor(0, 180, 0));
        
        if (i == bids.size() - 1)
            priceItem->setData(Qt::UserRole, "bottom_bid");
        
        orderBooksTable->setItem(row, 0, priceItem);
        
        QProgressBar* bar = new QProgressBar;
        double percentage = (bids[i].bidSize / mxSize) * 100.0;
        bar->setRange(0, 100);
        bar->setValue(percentage);
        bar->setTextVisible(true);
        bar->setFormat(QString::number(bids[i].bidSize, 'f', 4));
        bar->setStyleSheet(
            "QProgressBar {"
            "border: 1px solid #ccffcc;"
            "border-radius: 3px;"
            "text-align: center;"
            "}"
            "QProgressBar::chunk {"
            "background-color: #51cf66;"
            "border-radius: 2px;"
            "}"
        );
        orderBooksTable->setCellWidget(row, 1, bar);
    }
}

void ViewerOrderBooks::updateDisplay(const QList<WebSocketParser::Ask>& asks, const QList<WebSocketParser::Bid>& bids) {
    if (!m_hasChannel)
        return;

    orderBooksTable->setRowCount(0);

    if (!asks.isEmpty() && !bids.isEmpty())
        updateSpread(asks.first().askPrice, bids.first().bidPrice);
    
    double maxSize = 0.0;
    for (const auto& ask : asks) 
        maxSize = qMax(maxSize, ask.askSize);
    for (const auto& bid : bids) 
        maxSize = qMax(maxSize, bid.bidSize);
    if (maxSize == 0.0) 
        maxSize = 1.0;

    updateAsks(asks, maxSize);
    updateBids(bids, maxSize);
}