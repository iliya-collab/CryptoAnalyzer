#include "Engine/StdTypes.hpp"

namespace Engine {

    QString tradeToString(TypesTrade t_trade) {
        switch (t_trade) {
        case TypesTrade::SPOT:
            return "SPOT";
        case TypesTrade::LINEAR:
            return "LINEAR";
        case TypesTrade::INVERSE:
            return "INVERSE";
        case TypesTrade::OPTION:
            return "OPTION";
        default:
            return "NONE";
            break;
        }
    }

    QString tradeToBaseEndpoint(TypesTrade t_trade) {
        switch (t_trade) {
        case TypesTrade::SPOT:
            return "wss://stream.bybit.com/v5/public/spot";
        case TypesTrade::LINEAR:
            return "wss://stream.bybit.com/v5/public/linear";
        default:
            return QString();
            break;
        }
    }

}