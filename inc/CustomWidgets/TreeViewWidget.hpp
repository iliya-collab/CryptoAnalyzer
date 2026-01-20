#pragma once

#include <QWidget>
#include <QTreeView>
#include <QStandardItemModel>

class TreeViewWidget : public QWidget {
    Q_OBJECT
public:

    explicit TreeViewWidget(QWidget *parent = nullptr);
    
    
    // Добавляет элемент item к последнему существующему элементу из пути path
    QStandardItem* addItem(const QString& path);
    // Добавляет элементы items к последнему существующему элементу из пути path
    void addItems(const QString& path, const QStringList &items);
    void removeItem(const QString& path);

    /* 
    Проверяет существует ли элемент по пути path
    Если существует, то вернет указатель на последний элемент в пути, 
    иначе, указатель на последний существующий элемент
    В параментр index сохраняется индекс последнего существующего элемента
    path = Bybit/spot/ticker
    */
    QStandardItem* itemExists(const QString& path, int& lastFoundIndex) const;
    // Ищет элемент с названием itemName у элемента parent
    QStandardItem* findItem(QStandardItem* parent, const QString& itemName) const;

    void clear();
    
    void expandAll();
    void collapseAll();
    
    QString selectedText() const;
    QStringList selectedPath() const;

    
signals:
    void itemClicked(const QString &text);
    void itemDoubleClicked(const QString &text);
    
private slots:
    void onItemClicked(const QModelIndex &index);
    void onItemDoubleClicked(const QModelIndex &index);
    
private:
    void setupUI();
    void setupModel();
    void setupConnections();
    
    QTreeView* m_treeView;
    QStandardItemModel* m_model;
};