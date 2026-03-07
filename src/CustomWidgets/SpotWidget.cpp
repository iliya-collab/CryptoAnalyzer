#include "CustomWidgets/SpotWidget.hpp"

SpotWidget::SpotWidget(QWidget* parent) : QWidget(parent) {
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setupWidget();
}

void SpotWidget::selectedItem(const QModelIndex &index) {
    QString selectedCoin = index.data().toString();
    emit selected(selectedCoin);
}

void SpotWidget::changeFilter(int idBtn) {
    switch (idBtn)
    {
    case USDC_FILTER:
        m_widgetList->updateView(m_filtredLists["USDC"]);
        break;
    case USDT_FILTER:
        m_widgetList->updateView(m_filtredLists["USDT"]);
        break;
    case USDE_FILTER:
        m_widgetList->updateView(m_filtredLists["USDE"]);
        break;
    case MNT_FILTER:
        m_widgetList->updateView(m_filtredLists["MNT"]);
        break;
    case OTHER_FILTER:
        m_widgetList->updateView(m_filtredLists["Other"]);
        break;
    }
}

void SpotWidget::setupWidget() {
    m_widgetList = new ListWidget(this);

    QHBoxLayout* filterLayout = new QHBoxLayout;

    QPushButton* usdcFilter = new QPushButton("USDC");
    QPushButton* usdtFilter = new QPushButton("USDT");
    QPushButton* usdeFilter = new QPushButton("USDE");
    QPushButton* mntFilter = new QPushButton("MNT");
    QPushButton* otherFilter = new QPushButton("Other");

    m_btnGroup = new QButtonGroup(this);
    m_btnGroup->addButton(usdcFilter, USDC_FILTER);
    m_btnGroup->addButton(usdtFilter, USDT_FILTER);
    m_btnGroup->addButton(usdeFilter, USDE_FILTER);
    m_btnGroup->addButton(mntFilter, MNT_FILTER);
    m_btnGroup->addButton(otherFilter, OTHER_FILTER);

    filterLayout->addWidget(usdcFilter);
    filterLayout->addWidget(usdtFilter);
    filterLayout->addWidget(usdeFilter);
    filterLayout->addWidget(mntFilter);
    filterLayout->addWidget(otherFilter);

    connect(m_widgetList, &ListWidget::selected, this, &SpotWidget::selected);
    connect(m_btnGroup, &QButtonGroup::idClicked, this, &SpotWidget::changeFilter);

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->addLayout(filterLayout);
    m_mainLayout->addWidget(m_widgetList);

    setFixedSize(200, 300);
}

void SpotWidget::setTradingPairs(const QList<Engine::TradingInfo>& pairs) {
    QStringList usdcPairs, usdtPairs, usdePairs, mntPairs, otherPairs;

    for (const auto& next_pair : pairs)
        if (next_pair.quote_coin == "USDC")
            usdcPairs.append(next_pair.symbol);
        else if (next_pair.quote_coin == "USDT")
            usdtPairs.append(next_pair.symbol);
        else if (next_pair.quote_coin == "USDE")
            usdePairs.append(next_pair.symbol);
        else if (next_pair.quote_coin == "MNT")
            mntPairs.append(next_pair.symbol);
        else
            otherPairs.append(next_pair.symbol);

    m_filtredLists["USDC"] = usdcPairs;
    m_filtredLists["USDT"] = usdtPairs;
    m_filtredLists["USDE"] = usdePairs;
    m_filtredLists["MNT"] = mntPairs;
    m_filtredLists["Other"] = otherPairs;

    m_widgetList->updateView(usdcPairs);
}