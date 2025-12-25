#include "JsonManager.hpp"

QJsonDocument JsonManager::doc = {};

std::expected<QJsonDocument, QString> JsonManager::readDocument(const char* _file) {

    QFile jsonFile(_file);

    if (!jsonFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return std::unexpected(QString("Cannot open file: %1").arg(jsonFile.errorString()));


    QByteArray jsonData = jsonFile.readAll();
    jsonFile.close();
    
    QJsonParseError parseError;
    doc = QJsonDocument::fromJson(jsonData, &parseError);

    if (parseError.error != QJsonParseError::NoError) 
        return std::unexpected(QString("JSON parse error: %1").arg(parseError.errorString()));
    
    if (!doc.isObject()) 
        return std::unexpected(QString("Invalid JSON structure"));

    return doc;
}

void JsonManager::writeDocument(const char* _file) {
    QFile jsonFile(_file);
    jsonFile.open(QIODevice::WriteOnly);
    jsonFile.write(doc.toJson(QJsonDocument::Indented));
    jsonFile.close();
    
}

std::expected<QJsonDocument, QString> JsonManager::isDocumentValid(const QJsonDocument& _doc)
{
    // Документ считается валидным и непустым если:
    // 1. Он не null
    // 2. Он не пустой
    // 3. Содержит либо непустой объект, либо непустой массив
    
    if (_doc.isNull()) 
        return std::unexpected(QString("Document isNull()"));
    
    if (_doc.isEmpty()) 
        return std::unexpected(QString("Document isEmpty()"));
    
    if (_doc.isObject()) {
        QJsonObject obj = doc.object();
        if (obj.isEmpty()) 
            return std::unexpected(QString("Object is empty"));
        return _doc;
    }
    
    if (_doc.isArray()) {
        QJsonArray arr = doc.array();
        if (doc.isEmpty())
            return std::unexpected(QString("Array is empty"));
        return _doc;
    }
    
    return std::unexpected(QString("Unknown document type"));
}

QJsonDocument JsonManager::getDocument() {
    return doc;
}

void JsonManager::setDocument(const QJsonDocument& _doc) {
    doc = _doc;
}