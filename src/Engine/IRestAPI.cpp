#include "Engine/IRestAPI.hpp"

const qint64 Engine::IRestAPI::TIMEOUT_REQUEST = 30000;

void Engine::IRestAPI::handleResponse() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    if (!reply) 
        return;
    
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray response = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(response);
        if (!doc.isNull()) 
            emit dataReceived(doc.object());
        else
            emit errorOccurred("Failed to parse JSON response");
    }
    else
        emit errorOccurred(reply->errorString());
    
    reply->deleteLater();
}