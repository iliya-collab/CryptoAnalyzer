#include "CustomWidgets/SpotWidget.hpp"

SpotWidget::SpotWidget(QWidget* parent) : CustomWidget(parent) {
    setupWidget();
}

void SpotWidget::updateListCoins(const QStringList& lst) {
    QStandardItemModel* currentModel = qobject_cast<QStandardItemModel*>(listCoins->model());

    if (!currentModel)
        return;

    currentModel->clear();
    for (const QString& coin : lst) {
        QStandardItem* item = new QStandardItem(coin);
        currentModel->appendRow(item);
    }
}

void SpotWidget::selectedItem(const QModelIndex &index) {
    QString selectedCoin = index.data().toString();
    //qDebug() << "Selected:" << selectedCoin;
    emit selected(selectedCoin);
}

void SpotWidget::setupWidget() {
    QStandardItemModel* model = new QStandardItemModel(m_widget);

    listCoins = new QListView(m_widget);
    listCoins->setViewMode(QListView::ListMode);
    listCoins->setSpacing(10);                      // Расстояние между элементами
    listCoins->setMovement(QListView::Static);      // Неподвижные элементы
    listCoins->setResizeMode(QListView::Adjust);    // Автоматическая подгонка

    listCoins->setModel(model);

    listCoins->setSelectionMode(QAbstractItemView::SingleSelection);
    listCoins->setEditTriggers(QAbstractItemView::NoEditTriggers);

    listCoins->setMinimumHeight(150);
    listCoins->setMaximumHeight(300);
    listCoins->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    connect(listCoins, &QListView::clicked, this, &SpotWidget::selectedItem);

    mainLayout = new QVBoxLayout(m_widget);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(listCoins);
}