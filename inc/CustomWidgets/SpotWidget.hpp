#pragma once

#include "CustomWidgets/CustomWidget.hpp"

#include <QStandardItemModel>
#include <QListView>
#include <QStringList>
#include <QVBoxLayout>

class SpotWidget : public CustomWidget {
    Q_OBJECT
private:

    QListView* listCoins;

    QVBoxLayout* mainLayout;

    void setupWidget() override;

private slots:

    void selectedItem(const QModelIndex &index);

public:

    void updateListCoins(const QStringList& lst);

    explicit SpotWidget(QWidget* parent = nullptr);

signals:

    void selected(const QString& item);

};