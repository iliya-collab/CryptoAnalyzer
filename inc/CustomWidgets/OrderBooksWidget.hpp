#pragma once

#include "CustomWidgets/CustomWidget.hpp"

#include "Engine/StdTypes.hpp"

#include <QBoxLayout>
#include <QTableWidget>
#include <QProgressBar>
#include <QHeaderView>
#include <QLabel>

class OrderBooksWidget : public CustomWidget {
    Q_OBJECT
protected:

    QVBoxLayout* mainLayout;

    QTableWidget* orderBooksTable;
    QLabel* lblSpread;

    void setupWidget() override;

    void setupOrderBooksTable();

    void updateSpread(double bestAsk, double bestBid);
    void updateAsks(const QList<Engine::Ask>& asks, double mxSize);
    void updateBids(const QList<Engine::Bid>& bids, double mxSize);

public:
        
    explicit OrderBooksWidget(QWidget* parent = nullptr);

    void updateDisplay(const QList<Engine::Ask>& asks, const QList<Engine::Bid>& bids);

};