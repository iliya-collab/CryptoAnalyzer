#pragma once

#include "CustomWidgets/ListWidget.hpp"
#include "Engine/StdTypes.hpp"

#include <QVBoxLayout>
#include <QPushButton>
#include <QButtonGroup>
#include <QHash>

class FuturesWidget : public QWidget {
    Q_OBJECT
private:

    enum {
        USDC_FILTER,
        USDT_FILTER,
        INVERSE_FILTER
    };

    QHash<QString, QStringList> m_filtredLists;

    ListWidget* m_widgetList;
    QButtonGroup* m_btnGroup;
    QVBoxLayout* m_mainLayout;

    void setupWidget();

private slots:

    void selectedItem(const QModelIndex &index);

    void changeFilter(int idBtn);

public:

    explicit FuturesWidget(QWidget* parent = nullptr);

    void setTradingPairs(const QList<Engine::TradingInfo>& linear, const QList<Engine::TradingInfo>& inverse);

signals:

    void selected(const QString& item);

};