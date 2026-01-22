#pragma once

#include <QDialog>

class IDialog : public QDialog {
    Q_OBJECT
protected:

    virtual void setupUI() = 0;

public:

    IDialog(QWidget* parent = nullptr) : QDialog(parent) {}
    virtual ~IDialog() = default;

};
