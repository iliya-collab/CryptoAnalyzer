#pragma once

#include <QComboBox>

class EditableComboBox : public QComboBox {
    Q_OBJECT
private:

    void setupComboBox();
    void setupConnections();

public:

    explicit EditableComboBox(QWidget* parent = nullptr) : QComboBox(parent) {
        setupComboBox();
        setupConnections();
    }

    void setInitItems(const QStringList& items);
    QStringList getAllItems() const;
    bool itemExists(const QString& text) const;
    bool addItemIfNotExists(const QString& text);

private slots:

    void onTextEdited(const QString& text);
    void onEditingFinished();
    void onReturnPressed();
    void onCurrentIndexChanged(int index);
    void onCurrentTextChanged(const QString& text);

signals:

    void textEdited(const QString& text);
    void editingFinished(const QString& text);
    void returnPressed(const QString& text);
    
    void itemSelected(const QString& text, int index);
    void currentTextChanged(const QString& text);

};