#include "CustomWidgets/TradeWidget.hpp"

#include <QLabel>
#include <QPushButton>

TradeWidget::TradeWidget(QWidget* parent) : QWidget(parent) {
    setupWidget();
}

void TradeWidget::setupWidget() {
    QLabel* lblTitle = new QLabel("Trading");
    lblTitle->setAlignment(Qt::AlignCenter);
    QPushButton* btnSpot = new QPushButton("Spot");
    QPushButton* btnFutures = new QPushButton("Futures");
    QPushButton* btnOptions = new QPushButton("Options");

    m_btnGroup = new QButtonGroup(this);
    m_btnGroup->addButton(btnSpot,      (int)idButtons::idSpotBtn);
    m_btnGroup->addButton(btnFutures,   (int)idButtons::idFuturesBtn);
    m_btnGroup->addButton(btnOptions,   (int)idButtons::idOptionsBtn);
    
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->addWidget(lblTitle);
    m_mainLayout->addWidget(btnSpot);
    m_mainLayout->addWidget(btnFutures);
    m_mainLayout->addWidget(btnOptions);

    connect(m_btnGroup, &QButtonGroup::buttonClicked, this, [this](QAbstractButton* button) {
        int id = m_btnGroup->id(button);
        qDebug() << "Selected id button:" << id;
    });

}