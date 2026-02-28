#pragma once

#include <QWidget>
#include <QStandardItemModel>
#include <QListView>
#include <QStringList>
#include <QVBoxLayout>

class SpotWidget : public QWidget {
    Q_OBJECT
private:

    QListView* listCoins;

    QVBoxLayout* mainLayout;

    void setupWidget();

private slots:

    void selectedItem(const QModelIndex &index);

public:

    void updateListCoins(const QStringList& lst);

    explicit SpotWidget(QWidget* parent = nullptr);

signals:

    void selected(const QString& item);

};