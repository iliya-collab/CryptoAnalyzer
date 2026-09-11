#pragma once
#include <Tools/Network/BaseRestAPI.hpp>
#include "IResponseHandler.hpp"
#include "ServiceConcepts.hpp"
#include <QObject>

namespace Core::Markets
{

class BasePrivateService : public IPrivateService
{
    Q_OBJECT

private:
    std::map<QString, std::unique_ptr<IPrivateResponseHandler>> m_handlers;

protected:

    const int LOADING_TIMEOUT = 30000;
    std::unique_ptr<Tools::BaseRestAPI> m_currentApi;

    // Метод для регистрации обработчика endpoint
    template<typename IHandler>
        requires HasEndpoint<IHandler>
    void registerHandler()
    {
        static_assert(std::is_base_of<IPrivateResponseHandler, IHandler>::value, "IHandler must inherit from IPrivateResponseHandler!");

        auto response = std::make_unique<IHandler>();

        if (!response)
            return;

        QString endpoint = IHandler::endpoint();
        m_handlers[endpoint] = std::move(response);
    }

    // Метод для GET запроса
    template<typename IHandler, typename... Args>
        requires HasEndpoint<IHandler> && IsGetRequest<IHandler, Args...>
    void requestGetImpl(Args&&... args)
    {
        if (!m_currentApi)
            return;
        auto params = IHandler::buildRequest(std::forward<Args>(args)...);
        m_currentApi->requestEndpointGet(IHandler::endpoint(), params, LOADING_TIMEOUT);
    }

    // Метод для POST запроса
    template<typename IHandler, typename... Args>
        requires HasEndpoint<IHandler> && IsPostRequest<IHandler, Args...>
    void requestPostImpl(Args&&... args)
    {
        if (!m_currentApi)
            return;
        auto params = IHandler::buildRequestBody(std::forward<Args>(args)...);
        m_currentApi->requestEndpointPost(IHandler::endpoint(), params, LOADING_TIMEOUT);
    }

    // Универсальный метод отправки сообщений
    template<typename Endpoint, typename... Args>
    void send(Args&&... args)
    {
        static_assert(HasEndpoint<Endpoint>, "No endpoint()");

        if (!m_currentApi)
            return;

        if constexpr (IsGetRequest<Endpoint, Args...>)
        {
            QUrlQuery params = Endpoint::buildRequest(std::forward<Args>(args)...);
            m_currentApi->requestEndpointGet(Endpoint::endpoint(), params, LOADING_TIMEOUT);
        }
        else if constexpr (IsPostRequest<Endpoint, Args...>)
        {
            QByteArray params = Endpoint::buildRequestBody(std::forward<Args>(args)...);
            m_currentApi->requestEndpointPost(Endpoint::endpoint(), params, LOADING_TIMEOUT);
        }
        else
        {
            static_assert(sizeof...(Args) < 0, "Endpoint doesn't match GET or POST signature");
        }
    }

public:

    explicit BasePrivateService(std::unique_ptr<Tools::BaseRestAPI> api, QObject* parent = nullptr)
        : IPrivateService(parent), m_currentApi(std::move(api))
    {
        connect(m_currentApi.get(), &Tools::BaseRestAPI::downloadProgress, this, &BasePrivateService::downloadProgress, Qt::UniqueConnection);
        connect(m_currentApi.get(), &Tools::BaseRestAPI::errorOccurred, this, &BasePrivateService::errorOccurred, Qt::UniqueConnection);
        connect(m_currentApi.get(), &Tools::BaseRestAPI::dataReceived, this, &BasePrivateService::onDataReceived, Qt::UniqueConnection);
    }

    virtual ~BasePrivateService() {}

protected slots:

    virtual void onDataReceived(const QUrl& reqUrl, const QByteArray& data)
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

