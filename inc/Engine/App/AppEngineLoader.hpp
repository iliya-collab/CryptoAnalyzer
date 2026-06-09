#pragma once

#include <QObject>
#include <QList>
#include <QString>
#include <QMutex>
#include <QTimer>
#include <functional>

#include "Engine/Tools/BybitRestAPI.hpp"
#include "Engine/Tools/DBHash.hpp"

namespace Engine {

    class AppEngineLoader : public QObject {
        Q_OBJECT

    private:
        // Асинхронные шаги загрузки
        void saveToDatabaseAsync();
        void requestTradingPairsAsync();
        void requestInfoAboutAccount();
        void cancelCurrentRequest();

        void execStep();
        void execNextStep();

        // загружает данные из БД
        void loadFromDatabase(const QString& category);
        // сохраняет данные в БД
        void saveToDatabase();

        void processSymbols(const QJsonObject& data); // парсит полученные символы

        // Загруженные торги
        QList<TradingInfo> m_tradingPairs;
        // Мьютекс для потокобезопасности
        mutable QMutex m_mutex;

        // Состояние загрузки
        QList<QPair<QString, std::function<void()>>> m_loadSteps;
        bool m_loading = false;
        qint64 m_totalSteps = 0;
        qint64 m_currentStep = 0;

        DBHash m_dbSystem;

        // Управление текущим запросом и таймаутом
        BybitRestAPI* m_currentApi = nullptr;
        static const int LOADING_TIMEOUT = 30000;

    public:
        explicit AppEngineLoader(QObject* parent = nullptr);
        ~AppEngineLoader();

        // Запускает асинхронную загрузку
        void startLoading();

        void setAPI(const API& api);
        void checkAPI();

        // Возвращает загруженные данные
        QList<TradingInfo> loadTradingPairs(const QString& category);

    signals:

        void progressChanged(const QString& stepName, int current, int total);
        void errorOccurred(const QString& error);
        void finished(bool success);

        void apiChecked(bool isValid);

    };

} // namespace Engine