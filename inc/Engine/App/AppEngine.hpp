#pragma once

//#include "Configs/PlatformConfig.hpp"
#include "Engine/BybitWebSocket.hpp"

#include <QObject>
 
namespace Engine {

    class AppEngine : public QObject {
        Q_OBJECT
    private:

        BybitWebSocket* m_webSocket;

    public:

        AppEngine(QObject* parent = nullptr);

        void run(const QString& coin, QList<BybitWebSocket::Stream> streams);

        void stop();

    signals:

        void launched();
        void stopped();
        void errorOccurred(const QString& error);

    };

}