#include "CustomWindowDialogs/DialogSetupMenu.hpp"

DialogSetupMenu::DialogSetupMenu(QWidget* parent) : IDialog(parent) {

    setWindowTitle("Settigs");
    setMinimumSize(700, 500);

    setupUI();
    connectionSignals();

    show();

}

void DialogSetupMenu::setupUI() {

    // Виджет содержания (меню)
    contentsWidget = new QListWidget(this);
    contentsWidget->setViewMode(QListView::IconMode);
    contentsWidget->setIconSize(QSize(64, 64));
    contentsWidget->setMovement(QListView::Static);
    contentsWidget->setMaximumWidth(150);
    contentsWidget->setSpacing(12);

    // stacked widget для страниц
    pagesWidget = new QStackedWidget(this);

    setup_platform_page = new SetupPlatformPage(this);
    setup_api_key_page = new SetupAPIKeyPage(this);

    setup_platform_page->createPage();
    setup_api_key_page->createPage();


    pagesWidget->addWidget(setup_platform_page->getPage());
    pagesWidget->addWidget(setup_api_key_page->getPage());

    QListWidgetItem* platformButton = new QListWidgetItem(contentsWidget);
    platformButton->setText("Platform");
    platformButton->setTextAlignment(Qt::AlignHCenter);
    platformButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem* APIKeyButton = new QListWidgetItem(contentsWidget);
    APIKeyButton->setText("API");
    APIKeyButton->setTextAlignment(Qt::AlignHCenter);
    APIKeyButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

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

    mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(contentsWidget);
    mainLayout->addLayout(rightLayout);

    contentsWidget->setCurrentRow(0);
}

void DialogSetupMenu::connectionSignals() {
    connect(contentsWidget, &QListWidget::currentItemChanged, this, &DialogSetupMenu::changePage);
    connect(applyButton, &QPushButton::clicked, this, &DialogSetupMenu::onClickedButtonApply);
}

void DialogSetupMenu::onClickedButtonApply() {
    setup_platform_page->readConfig();
    setup_api_key_page->readConfig();
}


void DialogSetupMenu::changePage(QListWidgetItem *current, QListWidgetItem *previous) {
    if (!current)
        current = previous;

    pagesWidget->setCurrentIndex(contentsWidget->row(current));
}
