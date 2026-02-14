#include "Engine/StdTypes.hpp"

QString Engine::marketToString(TMarket _market) {
    switch (_market)
    {
    case TMarket::SPOT:
        return "SPOT";
    case TMarket::LINEAR:
        return "LINEAR";
    case TMarket::INVERSE:
        return "INVERSE";
    case TMarket::OPTION:
        return "OPTION";
    default:
        return "NONE";
        break;
    }
}