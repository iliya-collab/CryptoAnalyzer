#include "Engine/App/AppEngine.hpp"

namespace Engine {

    AppEngine::AppEngine(QObject* parent) : QObject(parent) {
        m_webSocket = std::make_unique<BybitWebSocket>(this);

        // Обработка основных сигналов
        connect(m_webSocket.get(), &BybitWebSocket::connected, this, &AppEngine::started);
        connect(m_webSocket.get(), &BybitWebSocket::disconnected, this, [this]() {
            if (!m_isInterrupt)
                start();
            else {
                m_lastPair = "";
                emit stopped();
            }
        });
        connect(m_webSocket.get(), &BybitWebSocket::errorOccurred, this, &AppEngine::errorOccurred);

        connect(m_webSocket.get(), &BybitWebSocket::updatedTicker, this, [this](const stTicker& newTicker) {
            if (m_lastPair == newTicker.m_symbol)
                emit tickerUpdated(newTicker);
        });

        connect(m_webSocket.get(), &BybitWebSocket::updatedOrderbook, this, [this](const stOrderBook& newOrderBook) {
            if (m_lastPair == newOrderBook.m_symbol) {
                updateOrderbook(m_orderBooks[newOrderBook.m_symbol], newOrderBook);
                emit orderBookUpdated(m_orderBooks[newOrderBook.m_symbol]);
            }
        });

        connect(m_webSocket.get(), &BybitWebSocket::updatedKline, this, [this](const stKline& newKline) {
            if (m_lastPair == newKline.m_symbol) {
                m_savedCandles.append(newKline);
                emit klineUpdated(newKline);
            }
            else
                m_savedCandles.clear();
        });

        qDebug() << Q_FUNC_INFO << "created in:" << QThread::currentThread();
    }

    AppEngine::~AppEngine() {
        qDebug() << Q_FUNC_INFO << "launched from:" << QThread::currentThread();

        if (hasRunned())
            m_webSocket->close();

        qDebug() << Q_FUNC_INFO << "finished";
    }

    void AppEngine::updateOrderbook(stOrderBook& oldOrderbook, const stOrderBook& newOrderbook) {
        if (newOrderbook.m_type == "snapshot")
            snapshotOrderbook(oldOrderbook, newOrderbook);
        else
            deltaUpdateOrderbook(oldOrderbook, newOrderbook);
    }

    void AppEngine::snapshotOrderbook(stOrderBook& oldOrderbook, const stOrderBook& newOrderbook) {
        oldOrderbook.m_bids = newOrderbook.m_bids;
        oldOrderbook.m_asks = newOrderbook.m_asks;
    }

    void AppEngine::deltaUpdateOrderbook(stOrderBook& oldOrderbook, const stOrderBook& newOrderbook) {
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

    bool AppEngine::hasRunned() {
        return m_webSocket && m_webSocket->isOpen();
    }

    void AppEngine::setAPI(const API& api) {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        if (!m_webSocket)
            return;
        m_webSocket->initAPI(api);
    }

    void AppEngine::start() {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        if (!hasRunned())
            m_webSocket->open();
        else
            emit errorOccurred("The engine was started");
    }

    void AppEngine::stop(bool interrupt) {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        if (!hasRunned())
            emit errorOccurred("The engine failed to run");
        else {
            m_isInterrupt = interrupt;
            m_webSocket->close();
        }
    }

    void AppEngine::addTrade(const QString& pair) {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();
        m_lastPair = pair;
        if (!hasRunned())
            emit errorOccurred("The engine failed to run");
        else
            m_webSocket->subscribeToStream(m_lastPair, {
                BybitWebSocket::Stream::Ticker,
                BybitWebSocket::Stream::Orderbook,
                BybitWebSocket::Stream::Kline
            });
    }

}