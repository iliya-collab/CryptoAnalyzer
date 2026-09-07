#pragma once
#include "Tools/Network/BaseWebSocket.hpp"
#include <QTimer>
#include <QSet>
#include <QReadWriteLock>
#include <expected>

namespace Core::Tools {

    class BybitWebSocket : public BaseWebSocket
    {
        Q_OBJECT

    public:

        explicit BybitWebSocket(SocketType socketType, MarketType marketType = MarketType::Spot, QObject* parent = nullptr);
        ~BybitWebSocket() = default;

        void setMarketType(MarketType type) { m_marketType = type; }
        QString getMarketType() const { return marketTypeToString(m_marketType); }

        void init(const Api& api = Api()) override;

    private slots:

        void onConnected() override; // Обработка подключения
        void onDisconnected() override; // Обработка отключения
        void onError(QAbstractSocket::SocketError error) override; // Обработка ошибок при подключении
        void onSslErrors(const QList<QSslError>& errors) override; // Обработка ssl ошибок
        void onTextMessageReceived(const QString& message) override; // Обработка принятого сообщения
        void onBytesWritten(qint64 bytes) override;
        void onPing() override; // Вызывается по таймеру m_pingTimer

    private:

        void sendAuthMessage() override; // Отправка сообщения об авторизации
        void sendPingMessage() override; // Отправка сообщения о пинге

        void cleanupPingTimestamps();

        // Обработка входящих данных
        void processMessage(const QJsonObject& obj); // Обработка сообщения
        std::expected<QJsonObject, QString> parseTextMessage(const QString &message); // Превращает строковое сообщение в json объект

        // Криптография для приватного канала
        QString generateSignature(const QString& apiKey, const QString& apiSecret, const QString& expires);

        MarketType m_marketType;                    // Тип рынка
        QMap<QString, qint64> m_pingTimestamps; // req_id -> timestamp отправки
        double m_pingMs = 0;
        const int ACTIVE_PING_INTERVAL = 20000;

    };
    
}