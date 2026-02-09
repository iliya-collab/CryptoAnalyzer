#pragma once

#include "Engine/IRestAPI.hpp"

namespace Engine {
    class CryptoCompare : public IRestAPI {
        Q_OBJECT
    public:

        static const QString baseUrl;

        void requestEndpoint(const QString& endpoint, const QUrlQuery& params = QUrlQuery(), int timeout = -1) override;

        CryptoCompare(QObject* parent = nullptr) : IRestAPI(parent) {
            m_baseEndpoint = QString("https://min-api.cryptocompare.com");
        }
    };
    
} // namespace Engine
