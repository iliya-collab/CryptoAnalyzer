#pragma once

#include "CustomWidgets/CustomWidget.hpp"

#include <QStandardItemModel>
#include <QListView>
#include <QStringList>
#include <QVBoxLayout>

class CoinsWidget : public CustomWidget {
    Q_OBJECT
private:

    QListView* listCoins;

    QVBoxLayout* mainLayout;

    void setupWidget() override;

private slots:

    void selectedItem(const QModelIndex &index);

public:

    void updateListCoins(const QStringList& lst);
    void updateIcons(const QHash<QString, QByteArray>& icons);

    explicit CoinsWidget(QWidget* parent = nullptr);

signals:
    void selected(const QString& item);

};