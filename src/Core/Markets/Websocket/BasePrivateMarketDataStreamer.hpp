#pragma once
#include "Tools/Network/BaseWebSocket.hpp"
#include "IStreamHandler.hpp"
#include "IPrivateMarketDataStreamer.hpp"
#include "StreamerConcepts.hpp"

namespace Core::Markets
{

    class BasePrivateMarketDataStreamer : public IPrivateMarketDataStreamer
    {
        Q_OBJECT
    public:

        explicit BasePrivateMarketDataStreamer(std::unique_ptr<Tools::BaseWebSocket> websocket, QObject* parent = nullptr);

        void subscribe(QSet<PrivateStreams> streams) override;
        void unsubscribe(QSet<PrivateStreams> streams) override;

        void connectToStreams() override;
        void disconnectFromStreams() override;


    protected slots:

        virtual void onStarted() = 0;
        virtual void onStopped() = 0;
        virtual void onPingMeasured(qint64 pingMs) = 0;
        virtual void onErrorOccurred(const QString& error) = 0;
        virtual void onMessageReceived(const QJsonObject& message)
        {
            if (!message.contains("topic"))
                return;

            QString receivedTopic = message["topic"].toString();

            if (m_handlers.contains(receivedTopic))
                m_handlers[receivedTopic]->handle(message, this);
            else
                emit errorOccurred(id(), "Unknown topic: " + receivedTopic);
        }

    protected:

        template<typename IHandler>
            requires HasTopic<IHandler>
        void registerHandler()
        {
            static_assert(std::is_base_of<IPrivateStreamHandler, IHandler>::value, "IHandler must inherit from IPrivateStreamHandler!");

            auto responseToTopic = std::make_unique<IHandler>();

            if (!responseToTopic)
                return;

            QString topic = IHandler::topic();
            m_handlers[topic] = std::move(responseToTopic);
        }

        QString createStream(PrivateStreams stream);

        std::unique_ptr<Tools::BaseWebSocket> m_webSocket;
        QSet<QString> m_usedStreams;                                                    // Активные подписки (полные имена топиков)
        std::map<QString, std::unique_ptr<IPrivateStreamHandler>> m_handlers;           // Зарегистрированные обработчики

    };

}