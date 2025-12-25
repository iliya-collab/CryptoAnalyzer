#include <qcontainerfwd.h>
#pragma one

#include <QString>
#include <QList>

class RegularParser {
public:

    void parseString(const QString& str);

    void setSepChar(char ch) {
        sep = ch;
    }

    bool insertMultValues(QStringList& lst, const QString& expr);

private:


    char sep = ' ';

    QStringList ListTokens;
};
