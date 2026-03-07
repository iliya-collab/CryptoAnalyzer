#pragma once

#include <QObject>
#include <QList>
#include <QString>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>
#include <QMutex>

#include "Engine/StdTypes.hpp"

namespace Engine {

    class AppEngineLoader : public QObject {
        Q_OBJECT

    public:
        explicit AppEngineLoader(QObject* parent = nullptr);
        ~AppEngineLoader();

        // Основные методы
        void startLoading();
        QList<TradingInfo> getData(const QString& category) const;

    signals:
        void progressChanged(int current, int total);
        void stepStarted(const QString& stepName);
        void errorEngine(const QString& error);
        void tradingPairsReady(TMarket market);
        void finished(bool success);

    private:
        // Структура для шага загрузки
        struct LoadingStep {
            QString name;
            std::function<bool()> action;
        };

        // Метод выполняющийся в отдельном потоке
        bool runLoadingThread();

        // Методы загрузки
        bool loadConfigSync();
        bool loadTradingPairsSync(TMarket market);
        void getTradingPairs(TMarket market);
        void processSymbols(const QJsonObject& data);
        bool saveToDatabase();

        // Вспомогательные методы
        void handleError(const QString& error);
        void emitProgress(int current, int total);
        void emitStepStarted(const QString& step);

        // Данные
        QList<LoadingStep> m_loadSteps;
        QHash<QString, QList<TradingInfo>> m_tradingPairs;
        API m_api;
        
        // Для потокобезопасности
        mutable QMutex m_mutex;
        
        // Управление асинхронностью
        QFutureWatcher<bool>* m_futureWatcher;
        QAtomicInt m_progressCurrent;
        QAtomicInt m_progressTotal;
    };
}