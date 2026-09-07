#pragma once
#include "Tools/Network/BaseWebSocket.hpp"
#include "IStreamHandler.hpp"
#include "IPublicMarketDataStreamer.hpp"
#include "StreamerConcepts.hpp"

namespace Core::Markets
{

    class BasePublicMarketDataStreamer : public IPublicMarketDataStreamer
    {
        Q_OBJECT
    public:

        explicit BasePublicMarketDataStreamer(std::unique_ptr<Tools::BaseWebSocket> websocket, QObject* parent = nullptr);

        void subscribeSymbol(const QString& symbol, QSet<PublicStreams> streams) override;
        void unsubscribeSymbol(const QString& symbol, QSet<PublicStreams> streams) override;

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

            QString topic = message["topic"].toString();
            bool success = false;

            for (auto it = m_handlers.begin(); it != m_handlers.end(); ++it)
            {
                if (topic.startsWith(it->first))
                {
                    it->second->handle(message, this);
                    success = true;
                    break;
                }
            }

            if (!success)
                emit errorOccurred(id(), "Unknown topic: " + topic);
        }

    protected:

        template<typename IHandler>
            requires HasTopic<IHandler>
        void registerHandler()
        {
            static_assert(std::is_base_of<IPublicStreamHandler, IHandler>::value, "IHandler must inherit from IPublicStreamHandler!");

            auto responseToTopic = std::make_unique<IHandler>();

            if (!responseToTopic)
                return;

            QString topic = IHandler::topic();
            m_handlers[topic] = std::move(responseToTopic);
        }

        QString createStream(const QString& symbol, PublicStreams stream);

        std::unique_ptr<Tools::BaseWebSocket> m_webSocket;
        QSet<QString> m_usedStreams;                                            // Активные подписки (полные имена топиков)
        std::map<QString, std::unique_ptr<IPublicStreamHandler>> m_handlers;    // Зарегистрированные обработчики

    };

}