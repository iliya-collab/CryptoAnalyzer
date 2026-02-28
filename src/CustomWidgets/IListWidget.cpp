#include "CustomWidgets/IListWidget.hpp"

#include <QStandardItemModel>
#include <QStringList>

IListWidget::IListWidget(QWidget* parent) : QWidget(parent) {
    QStandardItemModel* model = new QStandardItemModel(this);

    m_listView = new QListView(this);
    m_listView->setViewMode(QListView::ListMode);

    m_listView->setModel(model);
    
    connect(m_listView, &QListView::clicked, this, &IListWidget::selectedItem);

    /*m_listView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_listView->setFixedSize(300, 300);
    m_listView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);*/

}

void IListWidget::updateView(const QStringList& listItems) {
    QStandardItemModel* currentModel = qobject_cast<QStandardItemModel*>(m_listView->model());

    if (!currentModel)
        return;

    currentModel->clear();
    for (const QString& nextItem : listItems) {
        QStandardItem* item = new QStandardItem(nextItem);
        currentModel->appendRow(item);
    }
}

void IListWidget::selectedItem(const QModelIndex& index) {
    QString item = index.data().toString();
    emit selected(item);
}