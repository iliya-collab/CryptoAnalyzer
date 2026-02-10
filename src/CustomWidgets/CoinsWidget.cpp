#include "CustomWidgets/CoinsWidget.hpp"

CoinsWidget::CoinsWidget(QWidget* parent) : CustomWidget(parent) {
    setupWidget();
}

void CoinsWidget::updateListCoins(const QStringList& lst) {
    QStandardItemModel* currentModel = qobject_cast<QStandardItemModel*>(listCoins->model());

    if (!currentModel)
        return;

    currentModel->clear();
    for (const QString& coin : lst) {
        QStandardItem* item = new QStandardItem(coin);
        currentModel->appendRow(item);
    }
}

void CoinsWidget::updateIcons(const QHash<QString, QByteArray>& icons) {

}

void CoinsWidget::selectedItem(const QModelIndex &index) {
    QString selectedCoin = index.data().toString();
    qDebug() << "Selected:" << selectedCoin;
    emit selected(selectedCoin);
}

void CoinsWidget::setupWidget() {
    QStandardItemModel* model = new QStandardItemModel(m_widget);

    listCoins = new QListView(m_widget);
    listCoins->setViewMode(QListView::ListMode);
    listCoins->setIconSize(QSize(64, 64));          // Размер иконок
    //listCoins->setGridSize(QSize(80, 80));          // Размер ячейки
    listCoins->setSpacing(10);                      // Расстояние между элементами
    listCoins->setMovement(QListView::Static);      // Неподвижные элементы
    listCoins->setResizeMode(QListView::Adjust);    // Автоматическая подгонка

    listCoins->setModel(model);

    listCoins->setSelectionMode(QAbstractItemView::SingleSelection);
    listCoins->setEditTriggers(QAbstractItemView::NoEditTriggers);

    listCoins->setMinimumHeight(150);
    listCoins->setMaximumHeight(300);
    listCoins->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    connect(listCoins, &QListView::clicked, this, &CoinsWidget::selectedItem);

    mainLayout = new QVBoxLayout(m_widget);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(listCoins);
}