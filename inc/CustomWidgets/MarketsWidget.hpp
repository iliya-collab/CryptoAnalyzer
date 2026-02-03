#pragma once

#include "CustomWidgets/CustomWidget.hpp"

#include <QButtonGroup>
#include <QRadioButton>
#include <QVBoxLayout>

class MarketsWidget :public CustomWidget {
    Q_OBJECT
public:

    enum IDButtons {
        IdRadioSpotMarket,
        IdRadioFuturesMarket
    };

    explicit MarketsWidget(QWidget* parent = nullptr);

    int getSelectedMarket() const;

signals:
    void marketChanged(int marketId);

private slots:
    void onMarketSelected(int id);

private:

    QButtonGroup* groupMarkets;
    QRadioButton* spotMarket;
    QRadioButton* futuresMarket;

    QVBoxLayout* mainLayout;

    void setupWidget() override;

};
