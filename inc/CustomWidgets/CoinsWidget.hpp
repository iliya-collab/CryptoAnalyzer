#pragma once

#include "CustomWidgets/CustomWidget.hpp"

#include <QStandardItemModel>
#include <QListView>
#include <QStringList>
#include <QVBoxLayout>

class CoinsWidget : public CustomWidget {
private:

    QStandardItemModel* model;
    QListView* listCoins;

    QVBoxLayout* mainLayout;

    void setupWidget() override;

public:

    void setList(const QStringList& _lst);

    explicit CoinsWidget(QWidget* parent = nullptr);

};