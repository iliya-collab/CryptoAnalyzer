#include "CustomWidgets/ListWidget.hpp"

ListWidget::ListWidget(QWidget* parent) : QWidget(parent) {
    QStandardItemModel* model = new QStandardItemModel(this);

    m_listView = new QListView(this);
    m_listView->setViewMode(QListView::ListMode);

    m_listView->setModel(model);
    
    connect(m_listView, &QListView::clicked, this, &ListWidget::selectedItem);

    m_listView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_listView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_listView);
}

void ListWidget::updateView(const QStringList& listItems) {
    QStandardItemModel* currentModel = qobject_cast<QStandardItemModel*>(m_listView->model());

    if (!currentModel)
        return;

    currentModel->clear();
    for (const QString& nextItem : listItems) {
        QStandardItem* item = new QStandardItem(nextItem);
        currentModel->appendRow(item);
    }
}

void ListWidget::selectedItem(const QModelIndex& index) {
    QString item = index.data().toString();
    emit selected(item);
}

/*QStringList ListWidget::items() {
    QStandardItemModel* currentModel = qobject_cast<QStandardItemModel*>(m_listView->model());

    if (!currentModel)
        return QStringList();

    QStringList lstItems;

    for (int row = 0; row < model->rowCount(); ++row) {
        QModelIndex index = model->index(row, 0);
        QString itemText = model->data(index, Qt::DisplayRole).toString();
        lstItems->append(itemText);
    }

    return lstItems;
}*/