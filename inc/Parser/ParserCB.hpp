#ifndef _PARSER_CB_
#define _PARSER_CB_

#include <QXmlStreamReader>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <QList>
#include <QByteArray>
#include <QDataStream>
#include <QBuffer>

#include "CacheData.hpp"

struct CurrencyRateCB {
    QString Date;
    QString ID;
    QString CharCode;
    QString Name;
    int Nominal;
    double Value;
};

class ParserCB : public QObject {
    Q_OBJECT
    
private:
    
    QList<CurrencyRateCB> ListCurrencyRates;

    QString url_request = "";
    QString date = "";

    QNetworkAccessManager* manager;

    void parseCurrencies(QList<CurrencyRateCB>& ListCurrencyRates, const QByteArray &data);
    CurrencyRateCB parseValute(QXmlStreamReader& xml);

public:

    ParserCB() {
        manager = new QNetworkAccessManager(this);
    }

    ~ParserCB() {
        delete manager;
    }

    void makeRequest(const QString& date_req = "");
    void makeRequest(const QString& date_req1, const QString& date_req2, const QString& id);

    QString getLastUrlRequest() {
        return url_request;
    }

    QString getLastDate() {
        return date;
    }

    QList<CurrencyRateCB> getCurrencyRates() {
        return ListCurrencyRates;
    }


    static const char* url_dailyXML;
    static const char* url_dynamicXML;

    // Функция для преобразования структуры в QByteArray
    static QByteArray serializeCurrencyRate(const CurrencyRateCB& rate);

    // Функция для чтения структуры из QByteArray
    static bool deserializeCurrencyRate(const QByteArray& byteArray, CurrencyRateCB& rate);

};

#endif