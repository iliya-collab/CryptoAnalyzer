#pragma once

#include <QWidget>
#include <QButtonGroup>
#include <QVBoxLayout>

class TradeWidget : public QWidget {
    Q_OBJECT
private:

    enum class idButtons {
        idSpotBtn,
        idFuturesBtn,
        idOptionsBtn
    };

    void setupWidget();

    QButtonGroup* m_btnGroup;
    QVBoxLayout* m_mainLayout;

public:

    explicit TradeWidget(QWidget* parent = nullptr);
    ~TradeWidget() = default;
};