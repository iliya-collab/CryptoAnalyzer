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
        QList<TradingInfo> getData(const QString& category);

    signals:
        void progressChanged(const QString& stepName, int current, int total);
        void errorOccurred(const QString& error);
        void finished(bool success);

    private:
        // Асинхронные шаги загрузки
        void loadConfigAsync();
        void saveToDatabaseAsync();
        void requestTradingPairsAsync(TypeTrade t_trade);
        void cancelCurrentRequest();

        void execStep();
        void execNextStep();

        // читает конфигурацию из файла
        void loadConfig();
        // загружает данные из БД
        void loadFromDatabase(const QString& category);
        // сохраняет данные в БД
        void saveToDatabase();
        
        void processSymbols(const QJsonObject& data); // парсит полученные символы

        // Данные об API
        QString m_apiKey;
        QString m_secretKey;
        bool m_testnet = false;

        // Загруженные торги
        QList<TradingInfo> m_tradingPairs;
        // Мьютекс для потокобезопасности
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