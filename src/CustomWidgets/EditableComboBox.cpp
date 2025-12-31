#include "CustomWidgets/EditableComboBox.hpp"

#include <QLineEdit>

void EditableComboBox::setupComboBox() {
    setEditable(true);
    lineEdit()->setPlaceholderText("Edit...");
}

void EditableComboBox::setupConnections() {
    // Сигнал при редактировании
    connect(lineEdit(), &QLineEdit::textEdited, this, &EditableComboBox::onTextEdited);
    
    // Сигнал при завершении редактирования (Enter или потеря фокуса)
    connect(lineEdit(), &QLineEdit::editingFinished, this, &EditableComboBox::onEditingFinished);
    
    // Сигнал при нажатии Enter
    connect(lineEdit(), &QLineEdit::returnPressed, this, &EditableComboBox::onReturnPressed);
    
    // Оригинальный сигнал комбобокса
    connect(this, &QComboBox::currentIndexChanged, this, &EditableComboBox::onCurrentIndexChanged);
    
    connect(this, &QComboBox::currentTextChanged, this, &EditableComboBox::onCurrentTextChanged);
}

void EditableComboBox::setInitItems(const QStringList& items) {
    clear();
    addItems(items);
}

QStringList EditableComboBox::getAllItems() const {
    QStringList items;
    for (int i = 0; i < count(); ++i)
        items << itemText(i);
    return items;
}

bool EditableComboBox::itemExists(const QString& text) const {
    return findText(text, Qt::MatchExactly) >= 0;
}

bool EditableComboBox::addItemIfNotExists(const QString& text) {
    if (!itemExists(text)) {
        addItem(text);
        return true;
    }
    return false;
}

void EditableComboBox::onTextEdited(const QString& text) {
    emit textEdited(text);
}

void EditableComboBox::onEditingFinished() {
    /*QString text = currentText().trimmed();
    if (!text.isEmpty()) {
        if (insertPolicy() != QComboBox::NoInsert && !itemExists(text)) {
            addItemIfNotExists(text);
            setCurrentText(text);
        }
        emit editingFinished(text);
        }*/
    QString text = currentText().trimmed();
    emit editingFinished(text);
}

void EditableComboBox::onReturnPressed() {
    /*QString text = currentText().trimmed();
    if (!text.isEmpty()) {
        if (!itemExists(text)) {
            addItem(text);
        }
        emit returnPressed(text);
        clearEditText();
    }*/
    QString text = currentText().trimmed();
    emit editingFinished(text);
}

void EditableComboBox::onCurrentIndexChanged(int index) {
    if (index >= 0) {
        QString text = itemText(index);
        emit itemSelected(text, index);
    }
}

void EditableComboBox::onCurrentTextChanged(const QString& text) {
    emit currentTextChanged(text);
}