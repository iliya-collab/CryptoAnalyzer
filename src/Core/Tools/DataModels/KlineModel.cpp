    #include "KlineModel.hpp"

    namespace Core::Tools {

        int KlineModel::rowCount(const QModelIndex &parent) const {
            return m_klineSeries.size();
        }

        QVariant KlineModel::data(const QModelIndex &index, int role) const {

            if (!index.isValid() || index.row() >= m_klineSeries.size())
                return QVariant();

            const auto& kline = m_klineSeries.at(index.row());

            switch (role) {
            case OpenRole:
                return kline.m_open;
            case CloseRole:
                return kline.m_close;
            case HighRole:
                return kline.m_high;
            case LowRole:
                return kline.m_low;
            case VolumeRole:
                return kline.m_volume;
            case TurnoverRole:
                return kline.m_turnover;
            case TimeRole:
                return kline.m_start;
            case ConfirmRole:
                return kline.m_confirm;
            default:
                return QVariant();
            };

        }

        QHash<int, QByteArray> KlineModel::roleNames() const {
            return {
                {OpenRole, "open"},
                {CloseRole, "close"},
                {HighRole, "high"},
                {LowRole, "low"},
                {VolumeRole, "volume"},
                {TurnoverRole, "turnover"},
                {TimeRole, "time"},
                {ConfirmRole, "isConfirm"}
            };
        }

        void KlineModel::updateKline(const Kline &kline)
        {
            if (m_klineSeries.isEmpty())
            {
                qDebug() << "First kline:" << QDateTime::fromMSecsSinceEpoch(kline.m_start).toLocalTime().toString("HH:mm");
                addKline(kline);
                return;
            }

            auto& lastKline = m_klineSeries.last();

            if (lastKline.m_confirm)
            {
                addKline(kline);
                return;
            }

            lastKline.m_close = kline.m_close;
            lastKline.m_high = std::max(lastKline.m_high, kline.m_high);
            lastKline.m_low = std::min(lastKline.m_low, kline.m_low);
            lastKline.m_volume = kline.m_volume;
            lastKline.m_turnover = kline.m_turnover;
            lastKline.m_start = kline.m_start;
            lastKline.m_end = kline.m_end;
            lastKline.m_confirm = kline.m_confirm;

            int lastRow = m_klineSeries.size() - 1;
            emit dataChanged(index(lastRow, 0), index(lastRow, 0),
                             {OpenRole, CloseRole, HighRole, LowRole, VolumeRole, TurnoverRole, TimeRole, ConfirmRole});
            emit modelUpdated();
        }

        void KlineModel::addKline(const Kline &kline)
        {
            int newRow = m_klineSeries.size();

            beginInsertRows(QModelIndex(), newRow, newRow);

            m_klineSeries.append(kline);
            m_length++;

            endInsertRows();

            emit lengthChanged();
            emit modelUpdated();
        }

        void KlineModel::addHistoricalKlines(const QList<Kline> &klines)
        {
            if (klines.isEmpty())
                return;

            int firstRow = 0;
            int lastRow = klines.size() - 1;

            beginInsertRows(QModelIndex(), firstRow, lastRow);

            for (const auto& kline : klines)
            {
                qDebug() << QDateTime::fromMSecsSinceEpoch(kline.m_start).toLocalTime().toString("HH:mm");
                m_klineSeries.prepend(kline);
            }

            m_length = m_klineSeries.size();

            endInsertRows();

            emit lengthChanged();
            emit modelUpdated();
        }

        void KlineModel::clear()
        {
            beginResetModel();
            m_klineSeries.clear();
            m_length = 0;
            endResetModel();
            emit lengthChanged();
        }

        QVariantMap KlineModel::get(int index) const
        {
            QVariantMap res{};

            if (index < 0 || index >= m_klineSeries.size())
                return res;

            auto item = m_klineSeries[index];

            res["open"] = item.m_open;
            res["close"] = item.m_close;
            res["high"] = item.m_high;
            res["low"] = item.m_low;
            res["volume"] = item.m_volume;
            res["turnover"] = item.m_turnover;
            res["time"] = item.m_start;
            res["isConfirm"] = item.m_confirm;

            return res;
        }

        QVariantMap KlineModel::last() const
        {
            return get(m_klineSeries.size() - 1);
        }

        QVariantMap KlineModel::first() const
        {
            return get(0);
        }

    }
