#pragma once
#include "Tools/StdTypes.hpp"

namespace Core::Markets
{

    class IMarketDataStreamer : public QObject
    {
        Q_OBJECT

    public:

        explicit IMarketDataStreamer(QObject* parent = nullptr) : QObject(parent) {}
        virtual ~IMarketDataStreamer() = default;

        virtual QString id() = 0;
        virtual void setApi(const Core::Tools::Api& api) = 0;
        virtual void start() = 0;
        virtual void stop() = 0;
        virtual void restart() = 0;
        virtual bool isRunning() = 0;

    signals:

        void started(const QString& id);
        void stopped(const QString& id);
        void pingMeasured(qint64 pingMs);
        void errorOccurred(const QString& id, const QString& error);

    };

}
