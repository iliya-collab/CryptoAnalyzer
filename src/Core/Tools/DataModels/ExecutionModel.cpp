#include "ExecutionModel.hpp"

int Core::Tools::ExecutionModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_executions.size();
}

QVariant Core::Tools::ExecutionModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_executions.size())
        return {};

    const auto& e = m_executions.at(index.row());

    switch (role)
    {
    case CategoryRole:
        return QVariant::fromValue(e.m_category);
    case SymbolRole:
        return e.m_symbol;
    case ExecIdRole:
        return e.m_execId;
    case OrderIdRole:
        return e.m_orderId;
    case OrderLinkIdRole:
        return e.m_orderLinkId;
    case SideRole:
        return QVariant::fromValue(e.m_side);
    case ExecPriceRole:
        return e.m_execPrice;
    case ExecQtyRole:
        return e.m_execQty;
    case ExecValueRole:
        return e.m_execValue;
    case ExecTypeRole:
        return QVariant::fromValue(e.m_execType);
    case ExecFeeRole:
        return e.m_execFee;
    case FeeCurrencyRole:
        return e.m_feeCurrency;
    case IsMakerRole:
        return e.m_isMaker;
    case ExecTimeRole:
        return e.m_execTime;
    default:
        return {};
    }

}

QHash<int, QByteArray> Core::Tools::ExecutionModel::roleNames() const
{
    static const QHash<int, QByteArray> roles = {
        { CategoryRole,    "category" },
        { SymbolRole,      "symbol" },
        { ExecIdRole,      "execId" },
        { OrderIdRole,     "orderId" },
        { OrderLinkIdRole, "orderLinkId" },
        { SideRole,        "side" },
        { ExecPriceRole,   "execPrice" },
        { ExecQtyRole,     "execQty" },
        { ExecValueRole,   "execValue" },
        { ExecTypeRole,    "execType" },
        { ExecFeeRole,     "execFee" },
        { FeeCurrencyRole, "feeCurrency" },
        { IsMakerRole,     "isMaker" },
        { ExecTimeRole,    "execTime" },
    };

    return roles;

}

void Core::Tools::ExecutionModel::addExecution(const ExecutionInfo &execution)
{
    beginInsertRows(QModelIndex(), 0, 0);
    m_executions.prepend(execution);
    endInsertRows();

    if (m_executions.size() > m_maxItem)
    {
        beginRemoveRows(QModelIndex(), m_executions.size() - 1, m_executions.size() - 1);
        m_executions.removeLast();
        endRemoveRows();
    }
}

void Core::Tools::ExecutionModel::addExecutionBatch(const QList<ExecutionInfo> &executions)
{
    if (executions.isEmpty())
        return;

    beginInsertRows(QModelIndex(), 0, executions.size() - 1);
    for (auto it = executions.crbegin(); it != executions.crend(); ++it)
        m_executions.prepend(*it);
    endInsertRows();

    trimIfNeeded();
}

QList<Core::Tools::ExecutionInfo> Core::Tools::ExecutionModel::executionsForOrder(const QString &orderId) const
{
    QList<ExecutionInfo> result;
    for (const auto& e : m_executions)
        if (e.m_orderId == orderId)
            result.append(e);
    return result;
}

void Core::Tools::ExecutionModel::setMaxItem(int max)
{
    max = std::max(1, max);

    if (m_maxItem == max)
        return;

    m_maxItem = max;

    emit maxItemChanged();
    trimIfNeeded();
}

void Core::Tools::ExecutionModel::trimIfNeeded()
{
    if (m_executions.size() <= m_maxItem)
        return;

    const int excess = m_executions.size() - m_maxItem;
    beginRemoveRows(QModelIndex(), m_executions.size() - excess, m_executions.size() - 1);
    m_executions.remove(m_executions.size() - excess, excess);
    endRemoveRows();
}
