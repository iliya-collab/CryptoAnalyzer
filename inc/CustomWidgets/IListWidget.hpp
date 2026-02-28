#pragma once

#include <QWidget>
#include <QListView>

class IListWidget : public QWidget {
    Q_OBJECT
private:

    QListView* m_listView;

private slots:

    void selectedItem(const QModelIndex &index);

public:

    void updateView(const QStringList& listItems);

    explicit IListWidget(QWidget* parent = nullptr);

signals:

    void selected(const QString& item);

};