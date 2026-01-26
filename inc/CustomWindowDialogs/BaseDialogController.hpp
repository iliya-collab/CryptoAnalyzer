#pragma once

#include "CustomWindowDialogs/IController.hpp"

template<typename TDialog>
class BaseDialogController : public IController {

    static_assert(std::is_base_of<IDialog, TDialog>::value, "TDialog must inherit from IDialog");
    
public:
    using DialogType = TDialog;
    
    explicit BaseDialogController(QWidget* parent = nullptr) : IController(parent) {}
    
    virtual ~BaseDialogController() {
        if (m_dialog)
            m_dialog->disconnect();
    }
    
    void create() override {
        if (!m_dialog) {
            m_dialog = std::make_unique<TDialog>(m_parent);
            connectDialogSignals();
            connect(this, &BaseDialogController::dialogCreated, this, &BaseDialogController::onDialogCreated);
            emit dialogCreated();
        }
    }

    void show() override {
        if (m_dialog) {
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

protected:
    
    void connectDialogSignals() {
        if (m_dialog)
            connect(m_dialog.get(), &QDialog::finished, this, &BaseDialogController::onDialogFinished);
    }
    
    void onDialogFinished(int result) {
        Q_UNUSED(result);
        m_dialog.release();
        m_dialog = nullptr;
        disconnect();
        emit dialogClosed();
    }

    virtual void onDialogCreated() = 0;

    std::unique_ptr<TDialog> m_dialog = nullptr;
};