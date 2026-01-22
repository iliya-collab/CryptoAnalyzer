#pragma once

#include "CustomWindowDialogs/IDialog.hpp"
#include "Parser/WebSocketParser.hpp"

#include <QComboBox>
#include <QBoxLayout>

class DialogOrderBook : public IDialog {
    Q_OBJECT
private:

    QComboBox* subscribedCoins;
    QComboBox* usedStockMarkets;

    QVBoxLayout* mainLayout;

public:

    explicit DialogOrderBook(QWidget* parent = nullptr);

    void setupUI() override;

};