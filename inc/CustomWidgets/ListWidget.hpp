#pragma once

#include <QWidget>
#include <QListView>
#include <QStandardItemModel>
#include <QStringList>
#include <QVBoxLayout>

class ListWidget : public QWidget {
    Q_OBJECT
private:

    QListView* m_listView;

private slots:

    void selectedItem(const QModelIndex &index);

public:

    void updateView(const QStringList& listItems);

    explicit ListWidget(QWidget* parent = nullptr);

signals:

    void selected(const QString& item);

};