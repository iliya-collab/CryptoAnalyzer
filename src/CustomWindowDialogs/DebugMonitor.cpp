#include "CustomWindowDialogs/DebugMonitor.hpp"

DebugMonitor::DebugMonitor(QWidget* parent) : IDialog(parent) {
    setWindowTitle("Debug");
    resize(400, 300);

    setupUI();

    connect(clearOutput, &QPushButton::clicked, this, [this]() { m_textEdit->clear(); });

    DebugOutput::instance()->redirectQtMessages();
    DebugOutput::instance()->setTextEdit(m_textEdit);
}

void DebugMonitor::setupUI() {
    m_textEdit = new QTextEdit(this);
    m_textEdit->setReadOnly(true);

    clearOutput = new QPushButton("Clear", this);

    QHBoxLayout* row = new QHBoxLayout;
    row->addStretch();
    row->addStretch();
    row->addWidget(clearOutput);

    mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_textEdit);
    mainLayout->addLayout(row);
}