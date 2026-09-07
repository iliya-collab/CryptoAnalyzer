#include "OrderModel.hpp"

int Core::Tools::OrderModel::rowCount(const QModelIndex &parent) const
{
   return parent.isValid() ? 0 : m_orders.size();
}

QVariant Core::Tools::OrderModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_orders.size())
        return QVariant();

    const auto& o = m_orders.at(index.row());

    switch (role)
    {
    case CategoryRole:
        return QVariant::fromValue(o.m_category);
    case OrderIdRole:
        return o.m_orderId;
    case OrderLinkIdRole:
        return o.m_orderLinkId;
    case SymbolRole:
        return o.m_symbol;
    case SideRole:
        return QVariant::fromValue(o.m_side);
    case OrderTypeRole:
        return QVariant::fromValue(o.m_orderType);
    case PriceRole:
        return o.m_price;
    case QtyRole:
        return o.m_qty;
    case StatusRole:
        return QVariant::fromValue(o.m_status);
    case PositionIdxRole:
        return o.m_positionIdx;
    case LeavesQtyRole:
        return o.m_leavesQty;
    case CumExecQtyRole:
        return o.m_cumExecQty;
    case CumExecValueRole:
        return o.m_cumExecValue;
    case CumExecFeeRole:
        return o.m_cumExecFee;
    case RejectReasonRole:
        return o.m_rejectReason;
    case CreatedTimeRole:
        return o.m_createdTime;
    case UpdatedTimeRole:
        return o.m_updatedTime;
    default:
        return {};
    }
}

QHash<int, QByteArray> Core::Tools::OrderModel::roleNames() const
{
    return {
        { CategoryRole,     "category" },
        { OrderIdRole,      "orderId" },
        { OrderLinkIdRole,  "orderLinkId" },
        { SymbolRole,       "symbol" },
        { SideRole,         "side" },
        { OrderTypeRole,    "orderType" },
        { PriceRole,        "price" },
        { QtyRole,          "qty" },
        { StatusRole,       "status" },
        { PositionIdxRole,  "positionIdx" },
        { LeavesQtyRole,    "leavesQty" },
        { CumExecQtyRole,   "cumExecQty" },
        { CumExecValueRole, "cumExecValue" },
        { CumExecFeeRole,   "cumExecFee" },
        { RejectReasonRole, "rejectReason" },
        { CreatedTimeRole,  "createdTime" },
        { UpdatedTimeRole,  "updatedTime" },
    };

}

void Core::Tools::OrderModel::upsertOrder(const OrderInfo &order)
{
    auto it = m_orderIdToRow.find(order.m_orderId);

    if (it != m_orderIdToRow.end())
    {
        int row = it.value();
        m_orders[row] = order;
        const QModelIndex idx = index(row);
        emit dataChanged(idx, idx);
        return;
    }

    const int row = m_orders.size();
    beginInsertRows(QModelIndex(), row, row);
    m_orders.append(order);
    m_orderIdToRow.insert(order.m_orderId, row);
    endInsertRows();
}

void Core::Tools::OrderModel::resetOrders(const QList<OrderInfo> &orders)
{
    beginResetModel();
    m_orders = orders;
    rebuildIndex();
    endResetModel();
}

void Core::Tools::OrderModel::removeOrder(const QString &orderId)
{
    auto it = m_orderIdToRow.find(orderId);

    if (it == m_orderIdToRow.end())
        return;

    const int row = it.value();
    beginRemoveRows(QModelIndex(), row, row);
    m_orders.removeAt(row);
    endRemoveRows();

    rebuildIndex();
}

Core::Tools::OrderInfo Core::Tools::OrderModel::get(int row) const
{
    if (row < 0 || row >= m_orders.size())
        return {};
    return m_orders.at(row);
}

void Core::Tools::OrderModel::rebuildIndex()
{
    m_orderIdToRow.clear();
    m_orderIdToRow.reserve(m_orders.size());
    for (int i = 0; i < m_orders.size(); i++)
        m_orderIdToRow.insert(m_orders.at(i).m_orderId, i);
}
