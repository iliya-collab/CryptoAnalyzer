#pragma once

#include "CustomWidgets/CustomWidget.hpp"

#include <QListWidget>

class TradeWidget : public CustomWidget {
private:

    void setupWidget() override;

public:

    TradeWidget(QWidget* parent = nullptr);
    ~TradeWidget() = default;
};
