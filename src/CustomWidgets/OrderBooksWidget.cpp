#include "CustomWidgets/OrderBooksWidget.hpp"

OrderBooksWidget::OrderBooksWidget(QWidget* parent) : CustomWidget(parent) {
    setupWidget();   
}

void OrderBooksWidget::setupWidget() {
    mainLayout = new QVBoxLayout(m_widget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    setLayout(mainLayout);

    setupOrderBooksTable();
}

void OrderBooksWidget::setupOrderBooksTable() {
    lblSpread = new QLabel(m_widget);

    lblSpread->setStyleSheet(
        "border-radius: 3px;"
        "qproperty-alignment: AlignCenter;"
        "qproperty-wordWrap: true;"
    );

    QFont spreadFont = lblSpread->font();
    spreadFont.setPointSize(9);
    spreadFont.setBold(true);
    lblSpread->setFont(spreadFont);

    lblSpread->setText("ASK: -  |  BID: -  |  SPREAD: -");

    orderBooksTable = new QTableWidget(m_widget);
    orderBooksTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
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
    
    mainLayout->addWidget(lblSpread);
    mainLayout->addWidget(orderBooksTable);
    
}

void OrderBooksWidget::updateSpread(double bestAsk, double bestBid) {
    double spread = bestAsk - bestBid;
    QString spreadText = QString("ASK: %1  |  BID: %2  |  SPREAD: %3")
                        .arg(bestAsk, 0, 'f', 2)
                        .arg(bestBid, 0, 'f', 2)
                        .arg(spread, 0, 'f', 2);
    
    lblSpread->setText(spreadText);
}

void OrderBooksWidget::updateAsks(const QList<Engine::Ask>& asks, double mxSize) {
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

void OrderBooksWidget::updateBids(const QList<Engine::Bid>& bids, double mxSize) {
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

void OrderBooksWidget::updateDisplay(const QList<Engine::Ask>& asks, const QList<Engine::Bid>& bids) {
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