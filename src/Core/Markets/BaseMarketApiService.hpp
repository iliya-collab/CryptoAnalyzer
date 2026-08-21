#pragma once
#include <Tools/Network/BaseRestAPI.hpp>
#include "IResponseStrategy.hpp"
#include <QObject>

namespace Core::Markets
{

    class BaseMarketApiService : public IContext
    {
        Q_OBJECT
    protected:

        std::map<QString, std::unique_ptr<IResponseStrategy>> m_strategies;
        std::unique_ptr<Tools::BaseRestAPI> m_currentApi;

        void registerStrategy(std::unique_ptr<IResponseStrategy> strategy)
        {
            if (!strategy)
                return;

            QString endpoint = strategy->targetEndpoint();
            m_strategies.insert_or_assign(endpoint, std::move(strategy));
        }

    public:

        explicit BaseMarketApiService(std::unique_ptr<Tools::BaseRestAPI> api, QObject* parent = nullptr)
            : IContext(parent), m_currentApi(std::move(api))
        {
            connect(m_currentApi.get(), &Tools::BaseRestAPI::downloadProgress, this, &BaseMarketApiService::downloadProgress, Qt::UniqueConnection);
            connect(m_currentApi.get(), &Tools::BaseRestAPI::errorOccurred, this, &BaseMarketApiService::errorOccurred, Qt::UniqueConnection);
            connect(m_currentApi.get(), &Tools::BaseRestAPI::dataReceived, this, &BaseMarketApiService::onDataReceived, Qt::UniqueConnection);
        }

        virtual ~BaseMarketApiService() {}

        virtual void setAPI(const Tools::API& api) = 0;

    protected slots:

        void onDataReceived(const QUrl& reqUrl, const QByteArray& data)
        {
            QString path = reqUrl.path();

            if (m_strategies.contains(path))
                m_strategies[path]->handle(QJsonDocument::fromJson(data).object(), this);
            else
                emit errorOccurred("Unknown endpoint: " + path);
        }

    signals:

        void errorOccurred(const QString& error);
        void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

    };

}

