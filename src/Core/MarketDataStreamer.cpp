#include "MarketDataStreamer.hpp"
#include <QDateTime>

namespace Core {

    MarketDataStreamer::MarketDataStreamer(QObject* parent) : QObject(parent) {
        m_webSocket = std::make_unique<Tools::BybitWebSocket>(this);

        // Обработка основных сигналов
        connect(m_webSocket.get(), &Tools::BybitWebSocket::connected, this, &MarketDataStreamer::started);

        connect(m_webSocket.get(), &Tools::BybitWebSocket::disconnected, this, [this]() {
            if (!m_isInterrupt)
                start();
            else {
                m_lastPair = "";
                emit stopped();
            }
        });

        connect(m_webSocket.get(), &Tools::BybitWebSocket::errorOccurred, this, &MarketDataStreamer::errorOccurred);

        connect(m_webSocket.get(), &Tools::BybitWebSocket::pingMeasured, this, [this](double pingMs) {
            if (pingMs < 50)
                qInfo() << "Ping:" << pingMs << "ms" << "- Great";
            else if (pingMs >= 50 && pingMs < 150)
                qInfo() << "Ping:" << pingMs << "ms" << "- Good";
            else if (pingMs >= 150 && pingMs < 300)
                qInfo() << "Ping:" << pingMs << "ms" << "- Middle";
            else if (pingMs > 300)
                qInfo() << "Ping:" << pingMs << "ms" << "- Bad";
        });

        connect(m_webSocket.get(), &Tools::BybitWebSocket::updatedTicker, this, [this](const Tools::Ticker& newTicker) {
            if (m_lastPair == newTicker.m_symbol)
                emit tickerUpdated(newTicker);
        });

        connect(m_webSocket.get(), &Tools::BybitWebSocket::updatedOrderbook, this, [this](const Tools::Orderbook& newOrderbook) {
            if (m_lastPair == newOrderbook.m_symbol) {
                updateOrderbook(m_orderBooks[newOrderbook.m_symbol], newOrderbook);
                emit orderBookUpdated(m_orderBooks[newOrderbook.m_symbol]);
            }
        });

        connect(m_webSocket.get(), &Tools::BybitWebSocket::updatedKline, this, [this](const Tools::Kline& newKline) {
            if (m_lastPair == newKline.m_symbol) {
                m_savedCandles.append(newKline);
                emit klineUpdated(newKline);
            }
            else
                m_savedCandles.clear();
        });

        qDebug() << Q_FUNC_INFO << "created in:" << QThread::currentThread();
    }

    MarketDataStreamer::~MarketDataStreamer() {
        qDebug() << Q_FUNC_INFO << "launched from:" << QThread::currentThread();

        if (hasRunned())
            m_webSocket->close();
    }

    void MarketDataStreamer::updateOrderbook(Tools::Orderbook& oldOrderbook, const Tools::Orderbook& newOrderbook) {
        if (newOrderbook.m_type == "snapshot")
            snapshotOrderbook(oldOrderbook, newOrderbook);
        else
            deltaUpdateOrderbook(oldOrderbook, newOrderbook);
    }

    void MarketDataStreamer::snapshotOrderbook(Tools::Orderbook& oldOrderbook, const Tools::Orderbook& newOrderbook) {
        oldOrderbook.m_bids = newOrderbook.m_bids;
        oldOrderbook.m_asks = newOrderbook.m_asks;
    }

    void MarketDataStreamer::deltaUpdateOrderbook(Tools::Orderbook& oldOrderbook, const Tools::Orderbook& newOrderbook) {
        for (const auto& bidVal : newOrderbook.m_bids.asKeyValueRange()) {
            double price = bidVal.first;
            double size = bidVal.second;

            if (qFuzzyIsNull(size) || size <= 0)
                oldOrderbook.m_bids.remove(price);
            else
                oldOrderbook.m_bids.insert(price, size);
        }

        for (const auto& askVal : newOrderbook.m_asks.asKeyValueRange()) {
            double price = askVal.first;
            double size = askVal.second;

            if (qFuzzyIsNull(size) || size <= 0)
                oldOrderbook.m_asks.remove(price);
            else
                oldOrderbook.m_asks.insert(price, size);
        }
    }

    bool MarketDataStreamer::hasRunned() {
        return m_webSocket && m_webSocket->isOpen();
    }

    qint64 MarketDataStreamer::getStartTime() {
        return m_startTime;
    }

    void MarketDataStreamer::setAPI(const Tools::API& api) {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        if (!m_webSocket)
            return;
        m_webSocket->initAPI(api);
    }

    void MarketDataStreamer::start() {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        m_startTime = QDateTime::currentMSecsSinceEpoch();
        if (!hasRunned())
            m_webSocket->open();
        else
            emit errorOccurred("The engine was started");
    }

    void MarketDataStreamer::stop(bool interrupt) {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        if (!hasRunned())
            emit errorOccurred("The engine failed to run");
        else {
            m_isInterrupt = interrupt;
            m_webSocket->close();
        }
    }

    void MarketDataStreamer::addTrade(const QString& pair) {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        m_lastPair = pair;
        if (!hasRunned())
            emit errorOccurred("The engine failed to run");
        else
            m_webSocket->subscribeToStream(m_lastPair, {
                Tools::BybitWebSocket::Stream::Ticker,
                Tools::BybitWebSocket::Stream::Orderbook,
                Tools::BybitWebSocket::Stream::Kline
            });
    }

}