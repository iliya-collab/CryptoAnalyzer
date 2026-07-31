#pragma once
#include <QAbstractProxyModel>
#include <QtQml>

namespace Core::Tools {

    class ReversedProxyModel : public QAbstractProxyModel {
        Q_OBJECT
        QML_ELEMENT

    public:
        using QAbstractProxyModel::QAbstractProxyModel;

        ReversedProxyModel(QObject* parent = nullptr);

        QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
        QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;

        int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        int columnCount(const QModelIndex &parent = QModelIndex()) const override;
        QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
        QModelIndex parent(const QModelIndex &child) const override;

        QHash<int, QByteArray> roleNames() const override;
        QVariant data(const QModelIndex &index, int role) const override;

        void setSourceModel(QAbstractItemModel *sourceModel) override;

    private slots:

        void onRowsInserted(const QModelIndex &parent, int first, int last);
        void onRowsRemoved(const QModelIndex &parent, int first, int last);
        void onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());
        void onModelReset();
    };

}