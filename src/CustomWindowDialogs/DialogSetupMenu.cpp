#include "CustomWindowDialogs/DialogSetupMenu.hpp"

DialogSetupMenu::DialogSetupMenu(QWidget* parent) : CustomQDialog(parent) {

    setWindowTitle("Settigs");
    setMinimumSize(700, 500);

    setupUI();
    connectionSignals();

    show();

}

void DialogSetupMenu::setupUI() {

    QHBoxLayout* mainLayout = new QHBoxLayout(this);

    // Виджет содержания (меню)
    contentsWidget = new QListWidget(this);
    contentsWidget->setViewMode(QListView::IconMode);
    contentsWidget->setIconSize(QSize(64, 64));
    contentsWidget->setMovement(QListView::Static);
    contentsWidget->setMaximumWidth(150);
    contentsWidget->setSpacing(12);

    // stacked widget для страниц
    pagesWidget = new QStackedWidget(this);

    setup_scaner_page = new SetupScanerPage(this);
    setup_scaner_page->createPage();

    setup_my_wallet_page = new SetupMyWalletPage(this);
    setup_my_wallet_page->createPage();

    pagesWidget->addWidget(setup_scaner_page->getPage());
    pagesWidget->addWidget(setup_my_wallet_page->getPage());

    QListWidgetItem* scanerButton = new QListWidgetItem(contentsWidget);
    scanerButton->setText("Scaner");
    scanerButton->setTextAlignment(Qt::AlignHCenter);
    scanerButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem* myWalletButton = new QListWidgetItem(contentsWidget);
    myWalletButton->setText("My wallet");
    myWalletButton->setTextAlignment(Qt::AlignHCenter);
    myWalletButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    // Добавляем виджеты в layout
    mainLayout->addWidget(contentsWidget);
    mainLayout->addWidget(pagesWidget, 1);

    applyButton = new QPushButton("Apply");
    applyButton->setAutoDefault(false);
    applyButton->setDefault(false);
    applyButton->setFocusPolicy(Qt::NoFocus);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(applyButton);

    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->addWidget(pagesWidget, 1);
    rightLayout->addLayout(buttonLayout);

    mainLayout->addLayout(rightLayout);

    contentsWidget->setCurrentRow(0);
}

void DialogSetupMenu::connectionSignals() {
    connect(contentsWidget, &QListWidget::currentItemChanged, this, &DialogSetupMenu::changePage);
    connect(applyButton, &QPushButton::clicked, this, &DialogSetupMenu::onClickedButtonApply);
}

void DialogSetupMenu::onClickedButtonApply() {
    setup_scaner_page->readConfig();
    setup_my_wallet_page->readConfig();
}


void DialogSetupMenu::changePage(QListWidgetItem *current, QListWidgetItem *previous) {
    if (!current)
        current = previous;

    pagesWidget->setCurrentIndex(contentsWidget->row(current));
}
