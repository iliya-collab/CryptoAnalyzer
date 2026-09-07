#pragma once
#include <Tools/Network/BaseRestAPI.hpp>
#include "IResponseHandler.hpp"
#include "ServiceConcepts.hpp"
#include <QObject>

namespace Core::Markets
{

class BaseOrderService : public IOrderService
{
    Q_OBJECT

private:
    std::map<QString, std::unique_ptr<IOrderResponseHandler>> m_handlers;

protected:

    const int LOADING_TIMEOUT = 30000;
    std::unique_ptr<Tools::BaseRestAPI> m_currentApi;

    // Метод для регистрации обработчика endpoint
    template<typename IHandler>
        requires HasEndpoint<IHandler>
    void registerHandler()
    {
        static_assert(std::is_base_of<IOrderResponseHandler, IHandler>::value, "IHandler must inherit from IOrderResponseHandler!");

        auto response = std::make_unique<IHandler>();

        if (!response)
            return;

        QString endpoint = IHandler::endpoint();
        m_handlers[endpoint] = std::move(response);
    }

    // Метод для запроса
    template<typename IHandler, typename... Args>
        requires HasEndpoint<IHandler> && HasBuildRequest<IHandler, Args...>
    void requestImpl(Args&&... args)
    {
        if (!m_currentApi)
            return;
        auto params = IHandler::buildRequest(std::forward<Args>(args)...);
        m_currentApi->requestEndpoint(IHandler::endpoint(), params, LOADING_TIMEOUT);
    }

public:

    explicit BaseOrderService(std::unique_ptr<Tools::BaseRestAPI> api, QObject* parent = nullptr)
        : IOrderService(parent), m_currentApi(std::move(api))
    {
        connect(m_currentApi.get(), &Tools::BaseRestAPI::downloadProgress, this, &BaseOrderService::downloadProgress, Qt::UniqueConnection);
        connect(m_currentApi.get(), &Tools::BaseRestAPI::errorOccurred, this, &BaseOrderService::errorOccurred, Qt::UniqueConnection);
        connect(m_currentApi.get(), &Tools::BaseRestAPI::dataReceived, this, &BaseOrderService::onDataReceived, Qt::UniqueConnection);
    }

    virtual ~BaseOrderService() {}

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

