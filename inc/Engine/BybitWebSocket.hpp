#pragma once

#include "Engine/WebSocket.hpp"

namespace Engine {

    class BybitWebSocket : public Engine::WebSocket {
        Q_OBJECT

    protected:

        void sendSubscriptionMessage(const QStringList &streams) override;
        void sendUnsubscriptionMessage(const QStringList &streams) override;

        void messageReceived(const QJsonObject &obj) override;
        
        void updateTicker(const QJsonObject &json) override;
        void updateOrderBooks(const QJsonObject &json) override;
        
        QString tickerStream(const QString &coin) override;
        QString orderBooksStream(const QString &coin) override;

    public:

        explicit BybitWebSocket(QObject *parent = nullptr) : WebSocket("Bybit", parent) {};
        virtual ~BybitWebSocket() {};

    };

    class BybitWebSocketSpot : public BybitWebSocket {
    public:
        explicit BybitWebSocketSpot(QObject *parent = nullptr) : BybitWebSocket(parent) {
            Url = QUrl("wss://stream.bybit.com/v5/public/spot");
        }
    };

    class BybitWebSocketFutures : public BybitWebSocket {
    public:
        explicit BybitWebSocketFutures(QObject *parent = nullptr) : BybitWebSocket(parent) {
            Url = QUrl("wss://stream.bybit.com/v5/public/linear");
        }
    };

}