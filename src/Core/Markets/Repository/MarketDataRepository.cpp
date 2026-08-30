#include "MarketDataRepository.hpp"
#include "Tools/Database/SqliteDatabaseManager.hpp"
#include "Tools/Repository/RepositoryCreater.hpp"

namespace Core::Markets {

    MarketDataRepository::MarketDataRepository(QObject* parent) : QObject(parent)
    {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        const QString dbFile = "db/crypto.db";

        auto& manager = Tools::SqliteDatabaseManager::instance();

        if (!manager.open(dbFile))
            throw std::invalid_argument(manager.error().toStdString());

        try {
            m_cryptoRep = std::move(Tools::RepositoryCreater::instance().createRepository<Tools::CryptoRepository>(dbFile, manager));
            m_klineRep = std::move(Tools::RepositoryCreater::instance().createRepository<Tools::KlineRepository>(dbFile, manager));
            m_tradeRep = std::move(Tools::RepositoryCreater::instance().createRepository<Tools::TradeRepository>(dbFile, manager));
        } catch (const std::exception& e) {
            qCritical() << e.what();
            throw;
        }
    }

    QList<Tools::TradeInfo> MarketDataRepository::loadAllFromCryptoRepository() {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        QMutexLocker locker(&m_mutex);
        if (!m_cryptoRep->selectTrades()) {
            emit errorOccurred(m_cryptoRep->error());
            return QList<Tools::TradeInfo>();
        }
        return m_cryptoRep->getSelectedData();
    }

    void MarketDataRepository::clearCryptoRepository() {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        QMutexLocker locker(&m_mutex);
        if (!m_cryptoRep->clear()) {
            emit errorOccurred(m_cryptoRep->error());
            return;
        }
    }

    void MarketDataRepository::saveToCryptoRepository(const QList<Tools::TradeInfo>& tradePairs) {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        QMutexLocker locker(&m_mutex);
        if (!m_cryptoRep->insertTrades(tradePairs)) {
            emit errorOccurred(m_cryptoRep->error());
            return;
        }
    }

    void MarketDataRepository::saveToKlineRepository(const Tools::Kline& newKline) {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        QMutexLocker locker(&m_mutex);
        if (!m_klineRep->insertKline(newKline))
            emit errorOccurred(m_klineRep->error());
    }

    void MarketDataRepository::saveToKlinesRepository(const QList<Tools::Kline>& newKlines) {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        QMutexLocker locker(&m_mutex);
        if (!m_klineRep->insertKlines(newKlines))
            emit errorOccurred(m_klineRep->error());
    }


}

