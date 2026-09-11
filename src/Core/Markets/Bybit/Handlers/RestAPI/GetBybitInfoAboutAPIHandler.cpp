#include "GetBybitInfoAboutAPIHandler.hpp"

namespace Core::Markets
{

    QString GetBybitInfoAboutAPIHandler::parseISOFormat(const QString &time)
    {
        QDateTime utcTime = QDateTime::fromString(time, Qt::ISODate);

        if (!utcTime.isValid())
        {
            qWarning() << "Could not recognize the date:" << time;
            return time;
        }

        if (utcTime.toMSecsSinceEpoch() == 0)
        {
            qDebug() << "The key is perpetual (tied to an IP address)";
            return "";
        }

        QDateTime localTime = utcTime.toLocalTime();

        return localTime.toString("yyyy-MM-dd");
    }

    void GetBybitInfoAboutAPIHandler::handle(const QJsonObject &data, IPrivateService *service)
    {
        if (!data.contains("retMsg") || data["retMsg"].toString() != "OK")
        {
            emit service->errorOccurred(QString("Error processing request [endpoint = %1]: " + data["retMsg"].toString()).arg(endpoint()));
            return;
        }

        Tools::ApiInfo info{};

        QJsonObject result = data["result"].toObject();

        info.m_readOnly = result["readOnly"].toVariant().toBool();
        info.m_expiredAt = parseISOFormat(result["expiredAt"].toString());

        QJsonArray ipsArr = result["ips"].toArray();
        info.m_ips.reserve(ipsArr.size());
        for (const auto& ip : std::as_const(ipsArr))
            info.m_ips.emplaceBack(ip.toString());

        QJsonObject permissionsObj = result["permissions"].toObject();

        if (permissionsObj.contains("ContractTrade"))
        {
            QJsonArray contractArr = permissionsObj["ContractTrade"].toArray();

            bool hasOrder = false;
            bool hasPosition = false;

            for (const auto& val : std::as_const(contractArr))
            {
                QString perm = val.toString();
                if (perm == "Order")
                    hasOrder = true;
                else if (perm == "Position")
                    hasPosition = true;
            }

            info.m_permissionContractTrade = hasOrder && hasPosition;
        }
        else
            info.m_permissionContractTrade = false;

        if (permissionsObj.contains("Spot"))
        {
            QJsonArray spotArr = permissionsObj["Spot"].toArray();

            for (const auto& val : std::as_const(spotArr))
            {
                QString perm = val.toString();
                if (perm == "SpotTrade")
                {
                    info.m_permissionContractTrade = true;
                    break;
                }
            }
        }
        else
            info.m_permissionContractTrade = false;

        if (permissionsObj.contains("Wallet"))
        {
            QJsonArray walletArr = permissionsObj["Wallet"].toArray();

            for (const auto& val : std::as_const(walletArr))
            {
                QString perm = val.toString();
                if (perm == "Withdraw")
                    info.m_permissionWithdraw = true;
                else if (perm == "AccountTransfer")
                    info.m_permissionAccountTransfer = true;
            }
        }
        else
            info.m_permissionContractTrade = false;

        emit service->infoAboutApiReceived(info);
    }

}