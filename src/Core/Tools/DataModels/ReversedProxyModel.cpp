#include "ReversedProxyModel.hpp"

namespace Core::Tools {

    ReversedProxyModel::ReversedProxyModel(QObject* parent)
        : QAbstractProxyModel(parent) {}

    void ReversedProxyModel::setSourceModel(QAbstractItemModel* sourceModel)
    {
        if (sourceModel) {
            disconnect(sourceModel, nullptr, this, nullptr);
        }
        QAbstractProxyModel::setSourceModel(sourceModel);
        if (sourceModel) {
            connect(sourceModel, &QAbstractItemModel::rowsInserted,
                    this, &ReversedProxyModel::onRowsInserted);
            connect(sourceModel, &QAbstractItemModel::rowsRemoved,
                    this, &ReversedProxyModel::onRowsRemoved);
            connect(sourceModel, &QAbstractItemModel::dataChanged,
                    this, &ReversedProxyModel::onDataChanged);
            connect(sourceModel, &QAbstractItemModel::modelReset,
                    this, &ReversedProxyModel::onModelReset);
        }
    }

    QModelIndex ReversedProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
    {
        if (!sourceIndex.isValid() || !sourceModel())
            return QModelIndex();
        int sourceRow = sourceIndex.row();
        int targetRow = sourceModel()->rowCount() - 1 - sourceRow;
        return index(targetRow, sourceIndex.column());
    }

    QModelIndex ReversedProxyModel::mapToSource(const QModelIndex &proxyIndex) const
    {
        if (!proxyIndex.isValid() || !sourceModel())
            return QModelIndex();
        int targetRow = proxyIndex.row();
        int sourceRow = sourceModel()->rowCount() - 1 - targetRow;
        return sourceModel()->index(sourceRow, proxyIndex.column());
    }

    int ReversedProxyModel::rowCount(const QModelIndex &parent) const
    {
        Q_UNUSED(parent)
        return sourceModel() ? sourceModel()->rowCount() : 0;
    }

    int ReversedProxyModel::columnCount(const QModelIndex &parent) const
    {
        Q_UNUSED(parent)
        return sourceModel() ? sourceModel()->columnCount() : 0;
    }

    QModelIndex ReversedProxyModel::index(int row, int column, const QModelIndex &parent) const
    {
        Q_UNUSED(parent)
        return createIndex(row, column);
    }

    QModelIndex ReversedProxyModel::parent(const QModelIndex &child) const
    {
        Q_UNUSED(child)
        return QModelIndex();
    }

    QHash<int, QByteArray> ReversedProxyModel::roleNames() const
    {
        return sourceModel() ? sourceModel()->roleNames() : QHash<int, QByteArray>();
    }

    QVariant ReversedProxyModel::data(const QModelIndex &proxyIndex, int role) const
    {
        if (!proxyIndex.isValid() || !sourceModel())
            return QVariant();
        QModelIndex sourceIndex = mapToSource(proxyIndex);
        return sourceModel()->data(sourceIndex, role);
    }

    // ----- Обработка сигналов источника -----

    void ReversedProxyModel::onRowsInserted(const QModelIndex &parent, int first, int last)
    {
        beginResetModel();
        endResetModel();
    }

    void ReversedProxyModel::onRowsRemoved(const QModelIndex &parent, int first, int last)
    {
        beginResetModel();
        endResetModel();
    }

    void ReversedProxyModel::onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
    {
        QModelIndex proxyTopLeft = mapFromSource(topLeft);
        QModelIndex proxyBottomRight = mapFromSource(bottomRight);
        emit dataChanged(proxyTopLeft, proxyBottomRight, roles);
    }

    void ReversedProxyModel::onModelReset()
    {
        beginResetModel();
        endResetModel();
    }

}