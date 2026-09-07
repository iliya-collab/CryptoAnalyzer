#pragma once
#include <Tools/StdTypes.hpp>
#include <QAbstractListModel>

namespace Core::Tools {

    class TradePairsModel : public QAbstractListModel {
        Q_OBJECT
        QML_ELEMENT

    public:

        enum Roles {
            SymbolRole = Qt::UserRole + 1,
            QuoteRole,
            BaseRole,
            CategoryRole
        };

        explicit TradePairsModel(QObject* parent = nullptr) : QAbstractListModel(parent) {}

        // Обязательные методы QAbstractListModel
        int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
        QHash<int, QByteArray> roleNames() const override;

        void updateTradePairs(const QList<TradeInfo>& pairs);
        auto size() { return m_pairs.size(); }

    private:

        QList<TradeInfo> m_pairs;

    };

}
