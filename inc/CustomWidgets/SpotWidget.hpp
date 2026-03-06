#pragma once

#include "CustomWidgets/ListWidget.hpp"
#include "Engine/StdTypes.hpp"

#include <QVBoxLayout>
#include <QPushButton>
#include <QButtonGroup>
#include <QHash>

class SpotWidget : public QWidget {
    Q_OBJECT
private:

    enum {
        USDC_FILTER,
        USDT_FILTER,
        USDE_FILTER,
        MNT_FILTER,
        OTHER_FILTER
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

    explicit SpotWidget(QWidget* parent = nullptr);

    void setTradingPairs(const QList<Engine::TradingInfo>& pairs);

signals:

    void selected(const QString& item);

};