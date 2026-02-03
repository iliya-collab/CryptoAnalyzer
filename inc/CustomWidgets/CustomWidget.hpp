#pragma once

#include <QWidget>

class CustomWidget : public QWidget {
protected:

    QWidget* m_widget;

    virtual void setupWidget() = 0;

public:

    QWidget* widget() {
        return m_widget;
    }

    explicit CustomWidget(QWidget* parent = nullptr) : QWidget(parent) {
        m_widget = new QWidget(this);
    }

    virtual ~CustomWidget() = default;

};