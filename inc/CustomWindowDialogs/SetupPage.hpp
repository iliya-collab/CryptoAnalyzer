#ifndef SETUP_PAGE
#define SETUP_PAGE

#include <QWidget>
#include <QBoxLayout>

class SetupPage : public QWidget {
    Q_OBJECT
public:

    SetupPage(QWidget* parent = nullptr) : QWidget(parent) {}

    SetupPage(const SetupPage&) = delete;
    SetupPage& operator=(const SetupPage&) = delete;

    virtual void createPage() = 0;
    virtual void readConfig() = 0;

    QWidget* getPage() {
        return this;
    }

};

#endif // SETUP_PAGE