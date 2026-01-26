#include "CustomWindowDialogs/ViewerOrderBooks.hpp"

ViewerOrderBooks::ViewerOrderBooks(QWidget* parent) : IDialog(parent) {
    setModal(false);
    setWindowTitle("Order Books");
    setMinimumSize(300, 200);
    resize(500, 400);

    setupUI();
}

void ViewerOrderBooks::setupUI() {
    subscribedCoins = new QComboBox(this);
    usedStockMarkets = new QComboBox(this);

    QHBoxLayout* row1 = new QHBoxLayout;
    row1->addWidget(usedStockMarkets);
    row1->addWidget(subscribedCoins);

    mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(row1);

    setupOrderBookTable();
}

void ViewerOrderBooks::setStockMarkets(const QStringList& lst) {
    usedStockMarkets->addItems(lst);
}

void ViewerOrderBooks::setPairs(const QStringList& lst) {
    subscribedCoins->addItems(lst);
}

QString ViewerOrderBooks::getCurrentStockMarkets() {
    return usedStockMarkets->currentText();
}

QString ViewerOrderBooks::getCurrentPair() {
    return subscribedCoins->currentText();
}

void ViewerOrderBooks::setupOrderBookTable() {
    orderBookTable = new QTableWidget(this);
    orderBookTable->setColumnCount(2);
    orderBookTable->setHorizontalHeaderLabels({"Prive", "Size"});
    
    orderBookTable->horizontalHeader()->setStretchLastSection(true);
    orderBookTable->verticalHeader()->setVisible(false);
    orderBookTable->setShowGrid(false);
    orderBookTable->setAlternatingRowColors(true);
    
    orderBookTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    orderBookTable->setColumnWidth(0, 100);
    orderBookTable->setColumnWidth(1, 150);

    orderBookTable->setStyleSheet(
        "QTableWidget {"
        "gridline-color: #ddd;"  // Цвет линий сетки
        "}"
        "QTableWidget::item {"
        "border-right: 1px solid #ccc;"  // Вертикальные разделители между колонками
        "padding-right: 5px;"
        "}"
        "QTableWidget::item:last {"
        "border-right: none;"  // У последней колонки убираем правую границу
        "}"
    );
    
    mainLayout->addWidget(orderBookTable);
}

void ViewerOrderBooks::addHorSeparatorWidget(int row) {
    orderBookTable->setSpan(row, 0, 1, 3);
    QFrame* hLine = new QFrame;
    hLine->setFrameShape(QFrame::HLine);
    hLine->setFrameShadow(QFrame::Sunken);
    hLine->setStyleSheet("foreground-color: #888; height: 2px;");
    hLine->setMinimumHeight(2);
    orderBookTable->setCellWidget(row, 0, hLine);
}

void ViewerOrderBooks::addVerSeparatorWidget(int row) {
    QFrame* vLine = new QFrame;
    vLine->setFrameShape(QFrame::VLine);
    vLine->setFrameShadow(QFrame::Sunken);
    vLine->setStyleSheet("foreground-color: #888;");
    vLine->setMaximumWidth(2);
    orderBookTable->setCellWidget(row, 1, vLine);
}

void ViewerOrderBooks::updateDisplay(const QList<WebSocketParser::Ask>& asks, const QList<WebSocketParser::Bid>& bids) {
    /*orderBookTable->setRowCount(0);
    
    double maxSize = 0;
    for (const auto& ask : asks) 
        maxSize = qMax(maxSize, ask.askSize);
    for (const auto& bid : bids) 
        maxSize = qMax(maxSize, bid.bidSize);

    maxSize = (maxSize == 0) ? 1000 : maxSize;

    for (int i = 0; i < asks.size(); ++i) {
        int row = orderBookTable->rowCount();
        orderBookTable->insertRow(row);
        
        QTableWidgetItem* priceItem = new QTableWidgetItem(QString::number(asks[i].askPrice, 'f', 2));
        priceItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        priceItem->setForeground(Qt::red);
        orderBookTable->setItem(row, 0, priceItem);
        
        addVerSeparatorWidget(row);
        
        double percentage = asks[i].askSize / maxSize * 100;
        
        QProgressBar* progressBar = new QProgressBar();
        progressBar->setRange(0, 100);
        progressBar->setValue(percentage);
        progressBar->setTextVisible(true);
        progressBar->setFormat(QString::number(asks[i].askSize));
        progressBar->setStyleSheet("QProgressBar {"
                                   "border: 1px solid #ccc;"
                                   "border-radius: 3px;"
                                   "text-align: center;"
                                   "}"
                                   "QProgressBar::chunk {"
                                   "background-color: #ff6b6b;"
                                   "}");
        
        orderBookTable->setCellWidget(row, 2, progressBar);
    }

    int separatorRow = orderBookTable->rowCount();
    orderBookTable->insertRow(separatorRow);
    addHorSeparatorWidget(separatorRow);
        
    for (int i = 0; i < bids.size(); ++i) {
        int row = orderBookTable->rowCount();
        orderBookTable->insertRow(row);
        
        QTableWidgetItem* priceItem = new QTableWidgetItem(QString::number(bids[i].bidPrice, 'f', 2));
        priceItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        priceItem->setForeground(Qt::green);
        orderBookTable->setItem(row, 0, priceItem);
        
        addVerSeparatorWidget(row);
        
        double percentage = bids[i].bidSize / maxSize * 100;
        
        QProgressBar* progressBar = new QProgressBar();
        progressBar->setRange(0, 100);
        progressBar->setValue(percentage);
        progressBar->setTextVisible(true);
        progressBar->setFormat(QString::number(bids[i].bidSize));
        progressBar->setStyleSheet("QProgressBar {"
                                   "border: 1px solid #ccc;"
                                   "border-radius: 3px;"
                                   "text-align: center;"
                                   "}"
                                   "QProgressBar::chunk {"
                                   "background-color: #51cf66;"
                                   "}");
        
        orderBookTable->setCellWidget(row, 2, progressBar);
    }*/
    orderBookTable->setRowCount(0);
    
    double maxSize = 0.0;
    for (const auto& ask : asks) 
        maxSize = qMax(maxSize, ask.askSize);
    for (const auto& bid : bids) 
        maxSize = qMax(maxSize, bid.bidSize);
    if (maxSize == 0.0) maxSize = 1.0;
    
    for (int i = 0; i < asks.size(); ++i) {
        int row = orderBookTable->rowCount();
        orderBookTable->insertRow(row);
        
        QTableWidgetItem* priceItem = new QTableWidgetItem(QString::number(asks[i].askPrice, 'f', 2));
        priceItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        priceItem->setForeground(Qt::red);
        
        if (i == 0)
            priceItem->setData(Qt::UserRole, "top_ask");
        
        orderBookTable->setItem(row, 0, priceItem);
        
        QProgressBar* bar = new QProgressBar;
        double percentage = (asks[i].askSize / maxSize) * 100.0;
        bar->setRange(0, 100);
        bar->setValue(static_cast<int>(percentage));
        bar->setTextVisible(true);
        bar->setFormat(QString::number(asks[i].askSize, 'f', 4));
        bar->setStyleSheet(
            "QProgressBar {"
            "border: 1px solid #ffcccc;"
            "border-radius: 3px;"
            "text-align: center;"
            "color: #ff6b6b;"
            "background-color: #fff5f5;"
            "}"
            "QProgressBar::chunk {"
            "background-color: #ff6b6b;"
            "border-radius: 2px;"
            "}"
        );
        orderBookTable->setCellWidget(row, 1, bar);
    }
    
    // Разделительная строка между ask и bid
    if (asks.size() > 0 && bids.size() > 0) {
        int separatorRow = orderBookTable->rowCount();
        orderBookTable->insertRow(separatorRow);
        
        orderBookTable->setSpan(separatorRow, 0, 1, 2);
        
        QTableWidgetItem* separatorItem = new QTableWidgetItem("─ ASK / BID ─");
        separatorItem->setTextAlignment(Qt::AlignCenter);
        separatorItem->setForeground(QColor(100, 100, 100));
        separatorItem->setBackground(QColor(240, 240, 240));
        separatorItem->setFlags(Qt::NoItemFlags);
        
        separatorItem->setData(Qt::UserRole, "separator");
        
        orderBookTable->setItem(separatorRow, 0, separatorItem);
        orderBookTable->setRowHeight(separatorRow, 24);
    }
    
    for (int i = 0; i < bids.size(); ++i) {
        int row = orderBookTable->rowCount();
        orderBookTable->insertRow(row);
        
        QTableWidgetItem* priceItem = new QTableWidgetItem(
            QString::number(bids[i].bidPrice, 'f', 2));
        priceItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        priceItem->setForeground(QColor(0, 180, 0));
        
        if (i == bids.size() - 1)
            priceItem->setData(Qt::UserRole, "bottom_bid");
        
        orderBookTable->setItem(row, 0, priceItem);
        
        QProgressBar* bar = new QProgressBar;
        double percentage = (bids[i].bidSize / maxSize) * 100.0;
        bar->setRange(0, 100);
        bar->setValue(static_cast<int>(percentage));
        bar->setTextVisible(true);
        bar->setFormat(QString::number(bids[i].bidSize, 'f', 4));
        bar->setStyleSheet(
            "QProgressBar {"
            "border: 1px solid #ccffcc;"
            "border-radius: 3px;"
            "text-align: center;"
            "color: #51cf66;"
            "background-color: #f5fff5;"  // Светло-зеленый фон
            "}"
            "QProgressBar::chunk {"
            "background-color: #51cf66;"
            "border-radius: 2px;"
            "}"
        );
        orderBookTable->setCellWidget(row, 1, bar);
    }
}