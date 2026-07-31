#pragma once
#include "../StdTypes.hpp"
#include <QAbstractListModel>
#include <QQueue>

namespace Core::Tools {

    class TradeModel : public QAbstractListModel {
        Q_OBJECT
        QML_ELEMENT

    public:

        enum TradeRoles {
            TimeRole = Qt::UserRole + 1,
            PriceRole,
            VolumeRole,
            TurnoverRole,
            SideRole
        };

        explicit TradeModel(QObject* parent = nullptr) : QAbstractListModel(parent) {};

        int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
        QHash<int, QByteArray> roleNames() const override;

        void addTrade(const PublicTradeItem& item);
        void addTradeBatch(const QList<PublicTradeItem>& items);

    private:

        QList<PublicTradeItem> m_trades;
        int m_nTrades = 50;

    };

}
