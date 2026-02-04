#pragma once

#include "CustomWidgets/CustomWidget.hpp"

#include <QStandardItemModel>
#include <QListView>
#include <QStringList>
#include <QVBoxLayout>

class CoinsWidget : public CustomWidget {
    Q_OBJECT
private:

    QStandardItemModel* model;
    QListView* listCoins;

    QVBoxLayout* mainLayout;

    void setupWidget() override;

private slots:

    void selectedItem(const QModelIndex &index);

public:

    void setList(const QStringList& _lst);

    explicit CoinsWidget(QWidget* parent = nullptr);

signals:
    void selected(const QString& item);

};