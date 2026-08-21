#include "BybitKlineStrategy.hpp"

namespace Core::Markets
{
    QString BybitKlineStrategy::targetEndpoint() const { return "/v5/market/kline"; }

    void BybitKlineStrategy::handle(const QJsonObject &data, IContext *context)
    {
        QList<Tools::Kline> klines;
        processRequestKlines(klines, "1", data);
        emit context->klinesReceived(klines);
    }

    void BybitKlineStrategy::processRequestKlines(QList<Tools::Kline>& klines, const QString& interval, const QJsonObject& data)
    {
        QJsonObject result = data["result"].toObject();
        QString symbol = result["symbol"].toString();
        QJsonArray list = result["list"].toArray();

        for (const auto& obj : list) {
            QJsonArray itemArr = obj.toArray();
            Tools::Kline kline;
            kline.m_symbol = symbol;
            kline.m_start = itemArr[0].toString().toDouble();
            kline.m_end = kline.m_start + 60000;
            kline.m_open = itemArr[1].toString().toDouble();
            kline.m_high = itemArr[2].toString().toDouble();
            kline.m_low = itemArr[3].toString().toDouble();
            kline.m_close = itemArr[4].toString().toDouble();
            kline.m_volume = itemArr[5].toString().toDouble();
            kline.m_turnover = itemArr[6].toString().toDouble();
            kline.m_confirm = true;
            kline.m_interval = interval;
            klines.append(kline);
        }
    }
}
