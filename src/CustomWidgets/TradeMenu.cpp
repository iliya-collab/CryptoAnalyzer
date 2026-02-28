#include "CustomWidgets/TradeMenu.hpp"
#include "CustomWidgets/TradeWidget.hpp"
#include <QWidgetAction>

TradeMenu::TradeMenu(QWidget *parent) : QMenu(parent) {
    QWidgetAction* widgetAction = new QWidgetAction(this);
    TradeWidget* tradeWidget = new TradeWidget;
    widgetAction->setDefaultWidget(tradeWidget);
    addAction(widgetAction);
}