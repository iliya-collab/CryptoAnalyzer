#include "AssetModel.hpp"

namespace Core::Tools
{

    int AssetModel::rowCount(const QModelIndex &parent) const
    {
        return m_assets.size();
    }

    QVariant AssetModel::data(const QModelIndex &index, int role) const
    {
        if (!index.isValid() || index.row() >= m_assets.size())
            return QVariant();

        const auto& asset = m_assets[index.row()];

        switch (role)
        {
        case NameRole:
            return asset.name;
        case AmountRole:
            return asset.amount;
        case ValueRole:
            return asset.value;
        }

        return QVariant();
    }

    QHash<int, QByteArray> AssetModel::roleNames() const
    {
        return {
            {NameRole, "name"},
            {AmountRole, "amount"},
            {ValueRole, "value"}
        };
    }

    void AssetModel::updateAssets(std::vector<std::tuple<QString, double, double> > newAssets)
    {
        beginResetModel();
        m_assets.clear();
        for (const auto& [name, amount, value] : newAssets)
            m_assets.emplace_back(name, amount, value);
        endResetModel();
    }

}