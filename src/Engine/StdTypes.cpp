#include "Engine/StdTypes.hpp"

namespace Engine {

    QString tradeToString(TypeTrade t_trade) {
        switch (t_trade) {
        case TypeTrade::SPOT:
            return "spot";
        case TypeTrade::LINEAR:
            return "linear";
        case TypeTrade::INVERSE:
            return "inverse";
        case TypeTrade::OPTION:
            return "option";
        default:
            return "none";
            break;
        }
    }

    QString tradeToBaseEndpoint(TypeTrade t_trade) {
        switch (t_trade) {
        case TypeTrade::SPOT:
            return "wss://stream.bybit.com/v5/public/spot";
        case TypeTrade::LINEAR:
            return "wss://stream.bybit.com/v5/public/linear";
        default:
            return QString();
            break;
        }
    }

}