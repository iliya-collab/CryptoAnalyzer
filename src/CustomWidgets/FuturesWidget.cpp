#include "CustomWidgets/FuturesWidget.hpp"

FuturesWidget::FuturesWidget(QWidget* parent) : QWidget(parent) {
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setupWidget();
}

void FuturesWidget::selectedItem(const QModelIndex &index) {
    QString selectedCoin = index.data().toString();
    emit selected(selectedCoin);
}

void FuturesWidget::changeFilter(int idBtn) {
    switch (idBtn)
    {
    case USDC_FILTER:
        m_widgetList->updateView(m_filtredLists["USDC"]);
        break;
    case USDT_FILTER:
        m_widgetList->updateView(m_filtredLists["USDT"]);
        break;
    case INVERSE_FILTER:
        m_widgetList->updateView(m_filtredLists["INVERSE"]);
        break;
    }
}

void FuturesWidget::setupWidget() {
    m_widgetList = new ListWidget(this);

    QHBoxLayout* filterLayout = new QHBoxLayout;

    QPushButton* usdcFilter = new QPushButton("USDC");
    QPushButton* usdtFilter = new QPushButton("USDT");
    QPushButton* inverseFilter = new QPushButton("INVERSE");

    m_btnGroup = new QButtonGroup(this);
    m_btnGroup->addButton(usdcFilter, USDC_FILTER);
    m_btnGroup->addButton(usdtFilter, USDT_FILTER);
    m_btnGroup->addButton(inverseFilter, INVERSE_FILTER);

    filterLayout->addWidget(usdcFilter);
    filterLayout->addWidget(usdtFilter);
    filterLayout->addWidget(inverseFilter);

    connect(m_widgetList, &ListWidget::selected, this, &FuturesWidget::selected);
    connect(m_btnGroup, &QButtonGroup::idClicked, this, &FuturesWidget::changeFilter);

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->addLayout(filterLayout);
    m_mainLayout->addWidget(m_widgetList);

    setFixedSize(200, 300);
}

void FuturesWidget::setTradingPairs(const QList<Engine::TradingInfo>& linear, const QList<Engine::TradingInfo>& inverse) {
    QStringList usdcPairs, usdtPairs, inversePairs;

    for (const auto& next_pair : linear)
        if (next_pair.quote_coin == "USDC")
            usdcPairs.append(next_pair.symbol);
        else if (next_pair.quote_coin == "USDT")
            usdtPairs.append(next_pair.symbol);

    for (const auto& next_pair : inverse)
        inversePairs.append(next_pair.symbol);

    m_filtredLists["USDC"] = usdcPairs;
    m_filtredLists["USDT"] = usdtPairs;
    m_filtredLists["INVERSE"] = inversePairs;

    m_widgetList->updateView(usdcPairs);
}