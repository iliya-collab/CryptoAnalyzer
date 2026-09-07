#pragma once
#include "Tools/StdTypes.hpp"
#include <QAbstractListModel>

namespace Core::Tools
{

    class ExecutionModel : public QAbstractListModel
    {
        Q_OBJECT

        Q_PROPERTY(int maxItem READ maxItem WRITE setMaxItem NOTIFY maxItemChanged FINAL)

    public:

        enum Roles
        {
            CategoryRole = Qt::UserRole + 1,
            SymbolRole,
            ExecIdRole,
            OrderIdRole,
            OrderLinkIdRole,
            SideRole,
            ExecPriceRole,
            ExecQtyRole,
            ExecValueRole,
            ExecTypeRole,
            ExecFeeRole,
            FeeCurrencyRole,
            IsMakerRole,
            ExecTimeRole
        };
        Q_ENUM(Roles)

        explicit ExecutionModel(QObject* parent = nullptr) : QAbstractListModel(parent) {};

        int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
        QHash<int, QByteArray> roleNames() const override;

        // Добавляет один филл в начало списка (последние сделки сверху)
        void addExecution(const ExecutionInfo& execution);

        // Пакетная загрузка
        void addExecutionBatch(const QList<ExecutionInfo>& executions);

        // Возвращает все исполнения по конкретному ордеру
        Q_INVOKABLE QList<Core::Tools::ExecutionInfo> executionsForOrder(const QString& orderId) const;


        int maxItem() { return m_maxItem; }
        void setMaxItem(int max);

    signals:

        void maxItemChanged();

    private:

        QList<ExecutionInfo> m_executions{};
        int m_maxItem = 5000;

        void trimIfNeeded();

    };

}

