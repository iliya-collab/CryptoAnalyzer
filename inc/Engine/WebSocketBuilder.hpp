#pragma once

#include "Engine/WebSocket.hpp"
#include "Engine/BybitWebSocket.hpp"

#include <memory>

namespace Engine {

    class WebSocketBuilder {
    protected:
    
        WebSocketBuilder() = default;
        
        std::unique_ptr<WebSocket> m_webSocket;
    
    public:
    
        virtual ~WebSocketBuilder() = default;
    
        static WebSocketBuilder* create(const QString& StockMarket, const QString& Market);
        
        WebSocket* get();
    };

}
