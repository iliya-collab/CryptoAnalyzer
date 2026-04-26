#pragma once

#include <QObject>
#include <memory>

#include "Engine/Tools/BybitWebSocket.hpp"
 
namespace Engine {

    class AppEngine : public QObject {
        Q_OBJECT
    private:

        std::unique_ptr<BybitWebSocket> m_webSocket;

    public:

        AppEngine(QObject* parent = nullptr);
        ~AppEngine();

        void run(const QUrl& baseEndpont);

        void addTrade(const QString& pair);

        bool hasRunned();

        void stop();

    signals:

        void started();
        void stopped();
        void errorOccurred(const QString& error);

    };

}