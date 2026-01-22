#pragma once

#include "CustomWindowDialogs/IDialog.hpp"

#include <QObject>
#include <memory>

// Базовый интерфейс контроллера
class IController : public QObject {
    Q_OBJECT
    
public:
    explicit IController(QWidget* parent = nullptr) : QObject(parent), m_parent(parent) {}
    virtual ~IController() = default;
    
    // Возвращает диалог как IDialog*
    virtual IDialog* dialogInterface() = 0;
    
    
    virtual void show() = 0;
    virtual void hide() = 0;
    virtual bool isVisible() const = 0;
    
    virtual void update(const QVariant& data) = 0;

signals:
    void dialogCreated();
    void dialogShown();
    void dialogHidden();
    void dialogClosed();
    
protected:
    QWidget* m_parent = nullptr;
};

// Шаблонный базовый контроллер
template<typename TDialog>
class BaseDialogController : public IController {

    static_assert(std::is_base_of<IDialog, TDialog>::value, "TDialog must inherit from IDialog");
    
public:
    using DialogType = TDialog;
    
    explicit BaseDialogController(QWidget* parent = nullptr) : IController(parent) {}
    
    ~BaseDialogController() {
        if (m_dialog)
            m_dialog->disconnect();
    }
    
    IDialog* dialogInterface() override {
        return getDialog();
    }
    
    TDialog* getDialog() {
        if (!m_dialog)
            createDialog();
        return m_dialog.get();
    }
    
    void show() override {
        if (getDialog()) {
            m_dialog->show();
            emit dialogShown();
        }
    }
    
    void hide() override {
        if (m_dialog) {
            m_dialog->hide();
            emit dialogHidden();
        }
    }
    
    bool isVisible() const override {
        return m_dialog && m_dialog->isVisible();
    }
    
    void update(const QVariant& data) override {
        Q_UNUSED(data);
        // Базовая реализация
    }

protected:
    
    void connectDialogSignals() {
        // Подключаем стандартные сигналы QDialog
        if (m_dialog)
            connect(m_dialog.get(), &QDialog::finished, this, &BaseDialogController::onDialogFinished);
    }
    
    void onDialogFinished(int result) {
        Q_UNUSED(result);
        emit dialogClosed();
    }

private:
    void createDialog() {
        if (!m_dialog) {
            m_dialog = std::make_unique<TDialog>(m_parent);
            setupDialog(m_dialog.get());
            connectDialogSignals();
            emit dialogCreated();
        }
    }

protected:
    std::unique_ptr<TDialog> m_dialog = nullptr;
};