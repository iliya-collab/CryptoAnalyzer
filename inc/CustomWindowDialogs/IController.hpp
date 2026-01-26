#pragma once

#include "CustomWindowDialogs/IDialog.hpp"

#include <QObject>
#include <memory>

class IController : public QObject {
    Q_OBJECT
public:
    explicit IController(QWidget* parent = nullptr) : QObject(parent), m_parent(parent) {}
    virtual ~IController() = default;
    
    virtual void create() = 0;
    virtual void show() = 0;
    virtual void hide() = 0;
    virtual bool isVisible() const = 0;

signals:
    void dialogCreated();
    void dialogShown();
    void dialogHidden();
    void dialogClosed();
    
protected:
    QWidget* m_parent = nullptr;
};