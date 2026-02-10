#pragma once

#include "Engine/StdTypes.hpp"

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>

namespace Engine {

    class IRestAPI : public QObject {
        Q_OBJECT
    protected:

        QNetworkAccessManager* m_manager;
        QString m_baseEndpoint;

        void handleResponse();

    public:

        static const qint64 TIMEOUT_REQUEST;

        IRestAPI(QObject* parent = nullptr) : QObject(parent) {
            m_manager = new QNetworkAccessManager(parent);
        }
        virtual ~IRestAPI() = default;

        virtual void requestEndpoint(const QString& endpoint, const QUrlQuery& params = QUrlQuery(), int timeout = -1) = 0;

    signals:
    
        void dataReceived(const QJsonObject& obj);
        void errorOccurred(const QString &error);

    };

}