#include "CustomWidgets/TreeViewWidget.hpp"

#include <QVBoxLayout>
#include <QQueue>

TreeViewWidget::TreeViewWidget(QWidget *parent) : QWidget(parent) {
    setupUI();
    setupModel();
    setupConnections();
}

void TreeViewWidget::setupUI() {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    
    m_treeView = new QTreeView(this);
    m_treeView->setHeaderHidden(true); // Скрываем заголовок для простоты
    m_treeView->setAnimated(true);     // Включаем анимацию разворачивания
    m_treeView->setAlternatingRowColors(true); // Чередуем цвета строк
    
    layout->addWidget(m_treeView);
}

void TreeViewWidget::setupModel() {
    m_model = new QStandardItemModel(this);
    m_treeView->setModel(m_model);
}

void TreeViewWidget::setupConnections() {
    connect(m_treeView, &QTreeView::clicked, this, &TreeViewWidget::onItemClicked);
    connect(m_treeView, &QTreeView::doubleClicked, this, &TreeViewWidget::onItemDoubleClicked);
}

void TreeViewWidget::addItems(const QString& path, const QStringList& items) {
    QStandardItem* root = addItem(path);
    for (auto& it : items) {
        if (!findItem(root, it)) {
            QStandardItem *newItem = new QStandardItem(it);
            root->appendRow(newItem);
        }
    }

}

void TreeViewWidget::removeItem(const QString& path) {
    int i = 0;
    QStandardItem* item = itemExists(path, i);

    QStringList lst = path.split('/');
    
    if (!item || i != lst.size())
        return;

    
    QStandardItem* parent = item->parent();
    
   if (parent) {
        int row = item->row();
        parent->removeRow(row);
    } else {
        int row = item->row();
        m_model->removeRow(row);
    }
    
}

QStandardItem* TreeViewWidget::addItem(const QString& path) {
    int i = 0;
    QStandardItem* root = itemExists(path, i);

    QStringList lst = path.split('/');

    for (int j = i; j < lst.size(); j++) {
        QStandardItem *newItem = new QStandardItem(lst[j]);
        root->appendRow(newItem);
        root = newItem;
    }
    
    return root;
}

QStandardItem* TreeViewWidget::itemExists(const QString& path, int& lastFoundIndex) const {
    QStringList lst = path.split('/');
    if (lst.isEmpty()) {
        lastFoundIndex = 0;
        return m_model->invisibleRootItem();
    }
    
    QStandardItem* current = m_model->invisibleRootItem();
    int foundCount = 0;
    
    for (int i = 0; i < lst.size(); i++) {
        QStandardItem* found = findItem(current, lst[i]);
        if (!found) {
            lastFoundIndex = foundCount;
            return current;
        }
        
        current = found;
        foundCount++;
    }
    
    lastFoundIndex = foundCount;
    return current;
}

QStandardItem* TreeViewWidget::findItem(QStandardItem* parent, const QString& itemName) const {

    /*if (!parent)
        return nullptr;

    for (int row = 0; row < parent->rowCount(); row++) {
        QStandardItem* child = parent->child(row);
        if (child) {

            if (child->text() == itemName)
                return child;
        
            QStandardItem* found = findItem(child, itemName);
            if (found->text() == itemName)
                return found;

        }
    }
    
    return nullptr;*/

    if (!parent) 
        return nullptr;
    
    QQueue<QStandardItem*> queue;
    queue.enqueue(parent);
    
    while (!queue.isEmpty()) {
        QStandardItem* current = queue.dequeue();
        
        if (current->text() == itemName)
            return current;
        
        for (int row = 0; row < current->rowCount(); row++) {
            QStandardItem* child = current->child(row);
            if (child)
                queue.enqueue(child);
        }
    }
    
    return nullptr;
}

void TreeViewWidget::clear() {
    m_model->clear();
}

void TreeViewWidget::expandAll() {
    m_treeView->expandAll();
}

void TreeViewWidget::collapseAll() {
    m_treeView->collapseAll();
}

QString TreeViewWidget::selectedText() const {
    QModelIndex index = m_treeView->currentIndex();
    if (index.isValid())
        return m_model->data(index, Qt::DisplayRole).toString();
    return QString();
}

QStringList TreeViewWidget::selectedPath() const {
    QStringList path;
    QModelIndex index = m_treeView->currentIndex();
    
    while (index.isValid()) {
        path.prepend(m_model->data(index, Qt::DisplayRole).toString());
        index = index.parent();
    }
    
    return path;
}

void TreeViewWidget::onItemClicked(const QModelIndex &index) {
    if (index.isValid()) {
        QString text = m_model->data(index, Qt::DisplayRole).toString();
        emit itemClicked(text);
    }
}

void TreeViewWidget::onItemDoubleClicked(const QModelIndex &index) {
    if (index.isValid()) {
        QString text = m_model->data(index, Qt::DisplayRole).toString();
        emit itemDoubleClicked(text);
        
        if (m_model->hasChildren(index)) {
            bool isExpanded = m_treeView->isExpanded(index);
            m_treeView->setExpanded(index, !isExpanded);
        }
    }
}