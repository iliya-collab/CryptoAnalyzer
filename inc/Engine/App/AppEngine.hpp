#pragma once

#include <QObject>
#include <memory>

#include "Engine/Tools/BybitWebSocket.hpp"
 
namespace Engine {

    class AppEngine : public QObject {
        Q_OBJECT
    private:

        std::unique_ptr<BybitWebSocket> m_webSocket;

        bool m_isFilter;
        QString m_filter;

    public:

        AppEngine(QObject* parent = nullptr);
        ~AppEngine();

        void run();

        void setAPI(const API& api);

        void addTrade(const QString& pair);

        void enableFilter(const QString& pair, bool on = true);

        bool hasRunned();

        void stop();

    signals:

        void started();
        void stopped();
        void errorOccurred(const QString& error);

        void tickerUpdated(const stTicker& newTicker);
        void orderBookUpdated(const stOrderBook& newOrderBook);

    };

}