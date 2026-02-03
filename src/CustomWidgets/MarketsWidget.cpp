#include "CustomWidgets/MarketsWidget.hpp"

MarketsWidget::MarketsWidget(QWidget* parent) : CustomWidget(parent) {
    setupWidget();
}

void MarketsWidget::setupWidget() {
    QVBoxLayout* mainLayout = new QVBoxLayout(m_widget);

    spotMarket = new QRadioButton("Spot", this);
    futuresMarket = new QRadioButton("Futures", this);

    mainLayout->addWidget(spotMarket);
    mainLayout->addWidget(futuresMarket);
    
    groupMarkets = new QButtonGroup(this);
    groupMarkets->addButton(spotMarket, IdRadioSpotMarket);
    groupMarkets->addButton(futuresMarket, IdRadioFuturesMarket);

    spotMarket->setChecked(true);

    connect(groupMarkets, &QButtonGroup::idClicked, this, &MarketsWidget::onMarketSelected);
}

void MarketsWidget::onMarketSelected(int id) {
    if (id == IdRadioSpotMarket)
        qDebug() << "Spot market selected";
    else
        qDebug() << "Futures market selected";
    
    emit marketChanged(id);
}

int MarketsWidget::getSelectedMarket() const {
    return groupMarkets->checkedId();
}