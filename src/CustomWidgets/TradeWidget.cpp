#include "CustomWidgets/TradeWidget.hpp"
#include "CustomWidgets/SpotWidget.hpp"

#include <QLabel>
#include <QPushButton>
#include <QPropertyAnimation>

// https://share.google/aimode/STQlszaKLwumc4WRB

TradeWidget::TradeWidget(QWidget* parent) : QWidget(parent) {
    setupWidget();
    setupConnection();
}

void TradeWidget::setupWidget() {
    QLabel* lblTitle = new QLabel("Trading");
    lblTitle->setAlignment(Qt::AlignCenter);
    QPushButton* btnSpot = new QPushButton("Spot");
    QPushButton* btnFutures = new QPushButton("Futures");
    QPushButton* btnOptions = new QPushButton("Options");

    m_btnGroup = new QButtonGroup(this);
    m_btnGroup->addButton(btnSpot, idSpotBtn);
    m_btnGroup->addButton(btnFutures, idFuturesBtn);
    m_btnGroup->addButton(btnOptions, idOptionsBtn);
    
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->addWidget(lblTitle);
    m_mainLayout->addWidget(btnSpot);
    m_mainLayout->addWidget(btnFutures);
    m_mainLayout->addWidget(btnOptions);
}

void TradeWidget::setupConnection() {
    connect(m_btnGroup, &QButtonGroup::buttonClicked, this, [this](QAbstractButton* button) {
        int selectedID = m_btnGroup->id(button);
        QPushButton* btn = qobject_cast<QPushButton*>(button);
        
        if (selectedID == idSpotBtn) {
            SpotWidget* spot = new SpotWidget;
            spot->setAttribute(Qt::WA_DeleteOnClose);

            QPoint globalPos = btn->mapToGlobal(QPoint(btn->width() + 5, 0)); 

            spot->move(globalPos);
            spot->show();
            
            QPropertyAnimation* anim = new QPropertyAnimation(spot, "windowOpacity");
            anim->setDuration(200);
            anim->setStartValue(0.0);
            anim->setEndValue(1.0);
            anim->setEasingCurve(QEasingCurve::OutCubic);
            anim->start(QAbstractAnimation::DeleteWhenStopped);

            QPropertyAnimation* moveAnim = new QPropertyAnimation(spot, "pos");
            moveAnim->setDuration(200);
            moveAnim->setStartValue(globalPos + QPoint(20, 0));
            moveAnim->setEndValue(globalPos);
            moveAnim->setEasingCurve(QEasingCurve::OutBack);
            moveAnim->start(QAbstractAnimation::DeleteWhenStopped);
            
        };
    });
}
