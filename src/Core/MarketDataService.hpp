#pragma once
#include "MarketDataMediator.hpp"
#include "MarketDataState.hpp"
#include <QObject>

namespace Core {

    class MarketDataService : public QObject {
        Q_OBJECT

    private:

        std::shared_ptr<MarketDataState> m_state;
        std::shared_ptr<MarketDataMediator> m_mediator;

    public:

        explicit MarketDataService(std::shared_ptr<MarketDataState> state, std::shared_ptr<MarketDataMediator> mediator, QObject *parent = nullptr);

        Q_INVOKABLE void run();
        Q_INVOKABLE void restart();
        Q_INVOKABLE void shutdown();

        Q_INVOKABLE void subscribeSymbol(const QString& symbol);
        Q_INVOKABLE void loadTradePairs();
        Q_INVOKABLE void loadKlines(const QString& symbol, const QString& interval, qint64 start, qint64 end);
        Q_INVOKABLE void setAPI(const Tools::API& api);

    private slots:
        void onErrorOccurred(const QString& error);

    signals:

        // Уведомляет об ошибке
        void errorOccurred(const QString& error);
        // Уведомляет об прогрессе загрузки запроса
        void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
        // Уведомляет о старте потоковых данных
        void started();
        // Уведомляет о остановке потоковых данных
        void stopped();

        void messageReceived(const QString& msg);

    };


}


