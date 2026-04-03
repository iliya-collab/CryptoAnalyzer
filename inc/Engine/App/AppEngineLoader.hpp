#pragma once

#include <QObject>
#include <QList>
#include <QString>
#include <QMutex>
#include <QTimer>
#include <functional>

#include "Engine/StdTypes.hpp"
#include "Engine/Tools/BybitRestAPI.hpp"

namespace Engine {

    class AppEngineLoader : public QObject {
        Q_OBJECT

    public:
        explicit AppEngineLoader(QObject* parent = nullptr);
        ~AppEngineLoader();

        // Запускает асинхронную загрузку
        void startLoading();

        // Возвращает загруженные данные для указанной категории
        QList<TradingInfo> getData(const QString& category) const;

    signals:
        void progressChanged(const QString& stepName, int current, int total);
        void errorOccurred(const QString& error);
        void finished(bool success);

    private:
        // Асинхронные шаги загрузки
        void loadConfigAsync();
        void loadFromDatabaseAsync();
        void saveToDatabaseAsync();
        void requestTradingPairsAsync(Engine::TypesTrade trade);
        void cancelCurrentRequest();

        void execStep();
        void execNextStep();

        void loadConfig();                   // читает конфигурацию из файла
        void loadFromDatabase();             // загружает данные из БД
        void saveToDatabase();               // сохраняет данные в БД
        
        void processSymbols(const QJsonObject& data); // парсит полученные символы

        // Данные
        QString m_apiKey;
        QString m_secretKey;
        bool m_testnet = false;

        QHash<QString, QList<TradingInfo>> m_tradingPairs;
        mutable QMutex m_mutex;

        // Состояние загрузки
        QList<QPair<QString, std::function<void()>>> m_loadSteps;
        bool m_loading = false;
        qint64 m_totalSteps = 0;
        qint64 m_currentStep = 0;

        // Управление текущим запросом и таймаутом
        BybitRestAPI* m_currentApi = nullptr;
        static const int LOADING_TIMEOUT = 30000;
    };

} // namespace Engine