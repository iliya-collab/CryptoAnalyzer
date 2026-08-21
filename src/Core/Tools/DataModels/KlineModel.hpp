#pragma once
#include <Tools/StdTypes.hpp>
#include <QAbstractListModel>
#include <QtQml>

namespace Core::Tools {

    class KlineModel : public QAbstractListModel {
        Q_OBJECT
        QML_ELEMENT

        Q_PROPERTY(qint64 length READ length NOTIFY lengthChanged FINAL)

    public:

        enum Roles {
            OpenRole = Qt::UserRole + 1,
            CloseRole,
            HighRole,
            LowRole,
            VolumeRole,
            TurnoverRole,
            TimeRole,
            ConfirmRole
        };

        explicit KlineModel(QObject* parent = nullptr) : QAbstractListModel(parent) {};

        int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
        QHash<int, QByteArray> roleNames() const override;

        // Для потоковых данных
        // Обновляет последнюю открытую свечу в серии
        void updateKline(const Kline& kline);
        // Добавляет свечу в серию
        void addKline(const Kline& kline);

        // Для данных из сети
        // Добавляет исторические свичи в серию
        void addHistoricalKlines(const QList<Kline>& klines);

        qint64 length() { return m_length; }

        Q_INVOKABLE void clear();
        Q_INVOKABLE QVariantMap get(int index) const;
        Q_INVOKABLE QVariantMap last() const;
        Q_INVOKABLE QVariantMap first() const;

    signals:
        void lengthChanged();
        void modelUpdated();

    private:

        bool m_waitNewKline = true;
        qint64 m_length = 0;
        QList<Kline> m_klineSeries{};

    };

}
