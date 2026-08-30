#pragma once
#include "Tools/Network/BaseWebSocket.hpp"
#include "IStreamHandler.hpp"
#include "IMarketDataStreamer.hpp"

namespace Core::Markets
{

    class BaseMarketDataStreamer : public IMarketDataStreamer
    {
        Q_OBJECT
    public:

        explicit BaseMarketDataStreamer(std::unique_ptr<Tools::BaseWebSocket> websocket, QObject* parent = nullptr);

        void subscribeSymbol(const QString& symbol, QSet<WebSocketStreams> streams) override;
        void unsubscribeSymbol(const QString& symbol, QSet<WebSocketStreams> streams) override;

        void connectToStreams() override;
        void disconnectFromStreams() override;

    protected slots:

        virtual void onStarted() = 0;
        virtual void onStopped() = 0;
        virtual void onPingMeasured(qint64 pingMs) = 0;
        virtual void onErrorOccurred(const QString& error) = 0;
        virtual void onMessageReceived(const QJsonObject& message) = 0;

    protected:

        template<typename IHandler>
        void registerHandler(const QString& topic)
        {
            static_assert(std::is_base_of<IStreamHandler, IHandler>::value, "IHandler must inherit from IStreamHandler!");

            auto responseToTopic = std::make_unique<IHandler>();

            if (!responseToTopic)
                return;

            m_handlers[topic] = std::move(responseToTopic);
        }

        QString createStream(const QString& symbol, WebSocketStreams stream);

        std::unique_ptr<Tools::BaseWebSocket> m_webSocket;
        std::map<QString, std::unique_ptr<IStreamHandler>> m_handlers;  // Зарегистрированные обработчики


    private:

        QSet<QString> m_usedStreams;                                // Активные подписки (полные имена топиков)

    };

}