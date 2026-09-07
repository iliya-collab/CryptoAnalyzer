#pragma once
#include <Tools/StdTypes.hpp>
#include <QAbstractListModel>

namespace Core::Tools
{
    // Модель открытых позиций
    class PositionModel : public QAbstractListModel
    {
        Q_OBJECT

    public:

        enum Roles
        {
            CategoryRole = Qt::UserRole + 1,
            SymbolRole,
            SideRole,
            SizeRole,
            PositionIdxRole,
            AvgPriceRole,
            LiqPriceRole,
            MarkPriceRole,
            LeverageRole,
            PositionIMRole,
            PositionMMRole,
            UnrealisedPnlRole,
            CumRealisedPnlRole,
            AdlRankIndicatorRole
        };
        Q_ENUM(Roles)

        explicit PositionModel(QObject* parent = nullptr) : QAbstractListModel(parent) {};

        int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
        QHash<int, QByteArray> roleNames() const override;

        // Обновляет позицию по ключу (symbol, positionIdx); если её ещё нет — добавляет.
        // Если m_size == 0 (позиция закрыта), строка удаляется из модели
        void upsertPosition(const PositionInfo& position);

        void resetPositions(const QList<PositionInfo>& positions);

        Q_INVOKABLE Core::Tools::PositionInfo get(int row) const;
        Q_INVOKABLE Core::Tools::PositionInfo findBySymbol(const QString& symbol, int positionIdx = 0) const;

    private:

        struct PositionKey
        {
            QString symbol;
            int positionIdx = 0;

            bool operator==(const PositionKey& other) const
            {
                return symbol == other.symbol && positionIdx == other.positionIdx;
            }
        };

        static PositionKey keyOf(const PositionInfo& p) { return { p.m_symbol, p.m_positionIdx }; }

        QList<PositionInfo> m_positions;
        QHash<QString, int> m_keyToRow; // "symbol|positionIdx" -> индекс строки

        static QString hashKey(const PositionKey& key);
        void removeAt(int row);
        void rebuildIndex();

    };

}
