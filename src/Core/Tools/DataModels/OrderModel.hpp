#pragma once
#include <Tools/StdTypes.hpp>
#include <QAbstractListModel>

namespace Core::Tools
{

    // Модель ордеров пользователя
    class OrderModel : public QAbstractListModel
    {
        Q_OBJECT

    public:

        enum Roles
        {
            CategoryRole = Qt::UserRole + 1,
            OrderIdRole,
            OrderLinkIdRole,
            SymbolRole,
            SideRole,
            OrderTypeRole,
            PriceRole,
            QtyRole,
            StatusRole,
            PositionIdxRole,
            LeavesQtyRole,
            CumExecQtyRole,
            CumExecValueRole,
            CumExecFeeRole,
            RejectReasonRole,
            CreatedTimeRole,
            UpdatedTimeRole
        };
        Q_ENUM(Roles)

        explicit OrderModel(QObject* parent = nullptr) : QAbstractListModel(parent) {};

        int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
        QHash<int, QByteArray> roleNames() const override;

        // Добавляет ордер, если его orderId ещё нет, иначе обновляет существующую строку
        // и эмитит dataChanged только по изменённой строке (не сбрасывая всю модель).
        void upsertOrder(const OrderInfo& order);

        // Пакетная загрузка
        void resetOrders(const QList<OrderInfo>& orders);

        // Удаляет ордер из модели (например, когда терминальный статус больше не нужно держать в UI).
        void removeOrder(const QString& orderId);

        Q_INVOKABLE Core::Tools::OrderInfo get(int row) const;

    private:

        QList<OrderInfo> m_orders{};
        QHash<QString, int> m_orderIdToRow{};

        void rebuildIndex();

    };

}

