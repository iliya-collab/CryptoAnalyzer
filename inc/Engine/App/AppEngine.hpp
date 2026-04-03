#pragma once

//#include "Configs/PlatformConfig.hpp"
#include <QObject>

#include "Engine/Tools/BybitWebSocket.hpp"
 
namespace Engine {

    class AppEngine : public QObject {
        Q_OBJECT
    private:

        BybitWebSocket* m_webSocket;

    public:

        AppEngine(QObject* parent = nullptr);

        void run(const QUrl& baseEndpont);

        void stop();

    private slots:
    
        void onConnected();
        void onDisconnected();
        void onError(const QString& error);

    signals:

        void started();
        void stopped();
        void errorOccurred(const QString& error);

    };

}