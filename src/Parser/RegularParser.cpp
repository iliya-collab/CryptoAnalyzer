#include "Parser/RegularParser.hpp"
#include <QRegularExpression>

void RegularParser::parseString(const QString& str) {
    ListTokens = str.split(sep);
}

bool RegularParser::insertMultValues(QStringList& lst, const QString& expr) {

    QRegularExpression regex(R"(\$\{(\d+)\}\*(\d+))");
    QRegularExpressionMatchIterator it = regex.globalMatch(expr);

    if (it.hasNext()) {
        QRegularExpressionMatch match = it.next();

        if (match.captured(1).isEmpty() || match.captured(2).isEmpty())
            return false;

        bool ok1, ok2;
        auto duration = match.captured(1).toLongLong(&ok1);
        auto count = match.captured(2).toLongLong(&ok2);

        if (count <= 0 || duration <= 0)
            return false;

        for (qint64 i = 0; i < count; i++)
            lst.append(QString("%1").arg(duration));


        return true;
    }

    return false;
}
