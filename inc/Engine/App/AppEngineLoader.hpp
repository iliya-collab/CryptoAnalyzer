#pragma once

#include <QObject>
#include <QQueue>
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
        void errorOccurred(const QString& error);
        void tradingPairsReady(TypesTrade t_trade);
        void finished(bool success);

    private:
        // Структура для шага загрузки
        struct LoadingStep {
            QString name;
            std::function<bool()> action;
        };

        // Метод выполняющийся в отдельном потоке
        bool runLoadingThread();

        // Вспомогательные методы загрузки
        bool loadConfigSync();
        bool loadTradingPairsSync(TypesTrade t_trade);
        void getTradingPairs(TypesTrade t_trade);
        void processSymbols(const QJsonObject& data);
        // Методы загрузки
        bool loadFromDatabase();
        bool saveToDatabase();

        // Вспомогательные методы
        void emitError(const QString& error);
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
    };
}