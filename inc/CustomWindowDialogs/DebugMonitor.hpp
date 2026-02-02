#include "CustomWindowDialogs/DebugOutput.hpp"
#include "CustomWindowDialogs/IDialog.hpp"

#include <QBoxLayout>
#include <QPushButton>

class DebugMonitor : public IDialog {
private:
    
    QTextEdit* m_textEdit;
    QPushButton* clearOutput;

    QVBoxLayout* mainLayout;

    void setupUI() override;

public:

    DebugMonitor(QWidget* parent = nullptr);

};