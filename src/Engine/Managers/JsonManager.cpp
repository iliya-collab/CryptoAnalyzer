#include "Engine/Managers/JsonManager.hpp"

std::expected<QJsonDocument, QString> JsonManager::readDocument(const QString& name) {
    QFile jsonFile(name);

    if (!jsonFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return std::unexpected(jsonFile.errorString());


    QByteArray jsonData = jsonFile.readAll();

    jsonFile.close();

    QJsonParseError parseError;
    m_curDocument = QJsonDocument::fromJson(jsonData, &parseError);

    if (parseError.error != QJsonParseError::NoError)
        return std::unexpected(parseError.errorString());

    if (!m_curDocument.isObject())
        return isDocumentValid(m_curDocument);

    return m_curDocument;
}

std::optional<QString> JsonManager::writeDocument(const QString& name) {
    QFile jsonFile(name);

    if (jsonFile.open(QIODevice::WriteOnly)) {
        QByteArray data = m_curDocument.toJson(QJsonDocument::Indented);
        qint64 bytesWritten = jsonFile.write(data);

        if (bytesWritten == -1)
            return jsonFile.errorString();
        else if (bytesWritten != data.size())
            return "The data was not saved correctly";
    }

    jsonFile.close();

    return std::nullopt;
}

std::expected<QJsonDocument, QString> JsonManager::isDocumentValid(const QJsonDocument& doc)
{
    if (doc.isNull())
        return std::unexpected(QString("Document isNull()"));

    if (doc.isEmpty())
        return std::unexpected(QString("Document isEmpty()"));

    if (doc.isObject()) {
        QJsonObject obj = doc.object();
        if (obj.isEmpty())
            return std::unexpected(QString("Object is empty"));
        return doc;
    }

    if (doc.isArray()) {
        QJsonArray arr = doc.array();
        if (doc.isEmpty())
            return std::unexpected(QString("Array is empty"));
        return doc;
    }

    return std::unexpected(QString("Unknown document type"));
}

QJsonDocument JsonManager::getDocument() {
    return m_curDocument;
}

void JsonManager::setDocument(const QJsonDocument& doc) {
    m_curDocument = doc;
}
