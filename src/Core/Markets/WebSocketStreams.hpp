#pragma once

namespace Core::Markets
{

    enum class WebSocketStreams
    {
        Ticker,             // Публичный: Тикер
        Orderbook,          // Публичный: Стакан ордеров
        Kline,              // Публичный: Свечи
        PublicTrade         // Публичный: Трейд
    };

}