#pragma once

#include <QWidget>
#include <QButtonGroup>
#include <QVBoxLayout>

class TradeWidget : public QWidget {
    Q_OBJECT
private:

    enum {
        idSpotBtn,
        idFuturesBtn,
        idOptionsBtn
    };

    void setupWidget();

    void setupConnection();

    QButtonGroup* m_btnGroup;
    QVBoxLayout* m_mainLayout;

public:

    explicit TradeWidget(QWidget* parent = nullptr);
    ~TradeWidget() = default;
};