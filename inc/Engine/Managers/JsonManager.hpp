#ifndef JSON_MANAGER
#define JSON_MANAGER

#include <QFile>
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

#include <expected>

class JsonManager {
private:

    static QJsonDocument doc;

    JsonManager() = default;

public:

    static std::expected<QJsonDocument, QString> readDocument(const char* _file);
    static void writeDocument(const char* _file);

    static std::expected<QJsonDocument, QString> isDocumentValid(const QJsonDocument& _doc);
    static QJsonDocument getDocument();
    static void setDocument(const QJsonDocument& _doc);

};

#endif