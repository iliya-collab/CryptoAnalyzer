#pragma once

namespace Core::Markets
{
    // Публичные потоки
    enum class PublicStreams
    {
        Ticker,             // Тикер
        Orderbook,          // Стакан ордеров
        Kline,              // Свечи
        PublicTrade         // Трейд
    };

    // Приватные потокм
    enum class PrivateStreams
    {
        Wallet,             // Баланс кошелек
        Order,              // Отслеживание ордеров
        Position,           // Контроль позиции
        Execution           // История сделок
    };

}