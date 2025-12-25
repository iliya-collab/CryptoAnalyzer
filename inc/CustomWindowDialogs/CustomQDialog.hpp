#pragma once

#include <QDialog>
#include <QBoxLayout>

// Класс описывает основные функции, которые должны иметь все кастомные диалоговые окна
class CustomQDialog : public QDialog
{
    Q_OBJECT
protected:

    // Метод для настроики UI
    virtual void setupUI() = 0;
    // Метод для подклячения сигналов
    virtual void connectionSignals() = 0;

    // Главный layout кастомного диалого окна
    QVBoxLayout* layout = nullptr;

public:
    CustomQDialog(QWidget* parent = nullptr) : QDialog(parent) {}
    virtual ~CustomQDialog() {};
};
