#include "Engine/App/AppEngine.hpp"

namespace Engine {

    AppEngine::AppEngine(QObject* parent) : QObject(parent) {
        m_webSocket = std::make_unique<BybitWebSocket>(this);

        // Обработка основных сигналов
        connect(m_webSocket.get(), &BybitWebSocket::connected, this, &AppEngine::started);
        connect(m_webSocket.get(), &BybitWebSocket::disconnected, this, &AppEngine::stopped);
        connect(m_webSocket.get(), &BybitWebSocket::errorOccurred, this, &AppEngine::errorOccurred);

        connect(m_webSocket.get(), &BybitWebSocket::updatedTicker, this, [this](const stTicker& newTicker) {
            if (m_isFilter && (m_filter == newTicker.m_symbol)) {
                //qDebug().noquote() << "Latest update" << ticker.m_symbol << QTime::currentTime().toString() << '\n';
                emit tickerUpdated(newTicker);
            }
        });

        connect(m_webSocket.get(), &BybitWebSocket::updatedOrderbook, this, [this](const stOrderBook& newOrderBook) {
            if (m_isFilter && (m_filter == newOrderBook.m_symbol)) {
                //qDebug().noquote() << "Latest update" << ticker.m_symbol << QTime::currentTime().toString() << '\n';
                emit orderBookUpdated(newOrderBook);
            }
        });

        qDebug() << Q_FUNC_INFO << "created in:" << QThread::currentThread();
    }

    AppEngine::~AppEngine() {
        qDebug() << Q_FUNC_INFO << "launched from:" << QThread::currentThread();

        if (m_webSocket->isOpen())
            m_webSocket->close();

        qDebug() << Q_FUNC_INFO << "finished";
    }

    bool AppEngine::hasRunned() {
        return m_webSocket->isOpen();
    }

    void AppEngine::setAPI(const API& api) {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (!m_webSocket)
            return;

        m_webSocket->initAPI(api);
    }

    void AppEngine::run() {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (!m_webSocket)
            return;

        m_webSocket->open();
    }

    void AppEngine::stop() {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (m_webSocket->isOpen())
            m_webSocket->close();
    }

    void AppEngine::addTrade(const QString& pair) {
        qDebug() << Q_FUNC_INFO << "called from:" << QThread::currentThread();

        if (m_webSocket && m_webSocket->isOpen())
            m_webSocket->subscribeToStream(pair, {BybitWebSocket::Stream::Ticker});
    }

    void AppEngine::enableFilter(const QString& pair, bool on) {
        m_isFilter = on;
        m_filter = pair;
    }

}