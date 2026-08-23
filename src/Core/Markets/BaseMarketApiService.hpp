#pragma once
#include <Tools/Network/BaseRestAPI.hpp>
#include "IResponseHandler.hpp"
#include "ServiceConcepts.hpp"
#include <QObject>

namespace Core::Markets
{

    class BaseMarketApiService : public IMarketService
    {
        Q_OBJECT
\
    private:
        std::map<QString, std::unique_ptr<IResponseHandler>> m_handlers;

    protected:

        const int LOADING_TIMEOUT = 30000;
        std::unique_ptr<Tools::BaseRestAPI> m_currentApi;

        template<typename IHandler>
        requires HasEndpoint<IHandler>
        void registerHandler()
        {
            static_assert(std::is_base_of<IResponseHandler, IHandler>::value, "IHandler must inherit from IResponseHandler!");

            auto response = std::make_unique<IHandler>();

            if (!response)
                return;

            QString endpoint = IHandler::endpoint();
            m_handlers[endpoint] = std::move(response);
        }

        // Шаблонный метод для запроса торговых пар
        template<typename IHandler>
        requires HasTradePairsRequest<IHandler> && HasEndpoint<IHandler>
        void requestTradePairsImpl(const QString& category)
        {
            if (!m_currentApi)
                return;
            auto params = IHandler::buildRequest(category);
            m_currentApi->requestEndpoint(IHandler::endpoint(), params, LOADING_TIMEOUT);
        }

        // Шаблонный метод для запроса свечей
        template<typename IHandler>
        requires HasKlinesRequest<IHandler> && HasEndpoint<IHandler>
        void requestKlinesImpl(const QString& category, const QString& symbol, const QString& interval, qint64 start, qint64 end)
        {
            if (!m_currentApi)
                return;
            auto params = IHandler::buildRequest(category, symbol, interval, start, end);
            m_currentApi->requestEndpoint(IHandler::endpoint(), params, LOADING_TIMEOUT);
        }

        // Шаблонный метод для баланса
        template<typename IHandler>
        requires HasBalanceRequest<IHandler> && HasEndpoint<IHandler>
        void requestAccountBalanceImpl()
        {
            if (!m_currentApi)
                return;
            auto params = IHandler::buildRequest();
            m_currentApi->requestEndpoint(IHandler::endpoint(), params, LOADING_TIMEOUT);
        }

    public:

        explicit BaseMarketApiService(std::unique_ptr<Tools::BaseRestAPI> api, QObject* parent = nullptr)
            : IMarketService(parent), m_currentApi(std::move(api))
        {
            connect(m_currentApi.get(), &Tools::BaseRestAPI::downloadProgress, this, &BaseMarketApiService::downloadProgress, Qt::UniqueConnection);
            connect(m_currentApi.get(), &Tools::BaseRestAPI::errorOccurred, this, &BaseMarketApiService::errorOccurred, Qt::UniqueConnection);
            connect(m_currentApi.get(), &Tools::BaseRestAPI::dataReceived, this, &BaseMarketApiService::onDataReceived, Qt::UniqueConnection);
        }

        virtual ~BaseMarketApiService() {}

    protected slots:

        void onDataReceived(const QUrl& reqUrl, const QByteArray& data)
        {
            if (m_handlers.empty())
                return;

            QString path = reqUrl.path();

            if (m_handlers.contains(path))
                m_handlers[path]->handle(QJsonDocument::fromJson(data).object(), this);
            else
                emit errorOccurred("Unknown endpoint: " + path);
        }

    };

}

