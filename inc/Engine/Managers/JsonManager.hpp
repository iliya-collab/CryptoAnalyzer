#pragma once

#include <QFile>
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

#include <expected>
#include <optional>

class JsonManager {
private:

    QJsonDocument m_curDocument;

public:

    std::expected<QJsonDocument, QString> readDocument(const QString& name);
    std::optional<QString> writeDocument(const QString& name);

    std::expected<QJsonDocument, QString> isDocumentValid(const QJsonDocument& doc);
    QJsonDocument getDocument();
    void setDocument(const QJsonDocument& doc);

};