#include "BybitKlineHandler.hpp"

namespace Core::Markets
{
    void BybitKlineHandler::handle(const QJsonObject &data, IMarketService *service)
    {
        if (!data.contains("retMsg") || data["retMsg"].toString() != "OK")
        {
            emit service->errorOccurred(QString("Error processing request [endpoint = %1]: " + data["retMsg"].toString()).arg(endpoint()));
            return;
        }

        QList<Tools::Kline> klines;
        processRequestKlines(klines, "1", data);
        emit service->klinesReceived(klines);
    }

    void BybitKlineHandler::processRequestKlines(QList<Tools::Kline>& klines, const QString& interval, const QJsonObject& data)
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
