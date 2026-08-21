import QtQuick 2.15
import Application.Core 1.0
import Theme 1.0

Item {
    id: root

    readonly property real priceField: 0.25
    readonly property real volumeField: 0.25
    readonly property real turnoverField: 0.25
    readonly property real timeField: 0.25
    readonly property var recentTrades: ReversedProxyModel {
        sourceModel: AppCore.marketState.trades
    }

    ListView {
        id: tradeView
        anchors.fill: parent
        anchors.margins: Theme.margins
        model: root.recentTrades
        clip: true
        reuseItems: true

        delegate: Row {
            width: tradeView.width
            padding: Theme.padding

            property var priceStr: tradePrice ? tradePrice.toFixed(2) : "-"
            property var sideColor: tradeSide ? ((tradeSide === "Buy") ? "#0ECB81" : "#F6465D") : "#EAECEF"
            property var volumeStr: {
                if (!tradeVolume)
                    return "-"
                var volume = tradeVolume
                return volume >= 1000000 ? (volume / 1000000).toFixed(2) + "M" :
                        volume >= 1000 ? (volume / 1000).toFixed(2) + "K" :
                        volume.toFixed(2)
            }
            property var turnoverStr: {
                if (!tradeTurnover)
                    return "-"
                var turnover = tradeTurnover
                return turnover >= 1000000 ? (turnover / 1000000).toFixed(2) + "M" :
                        turnover >= 1000 ? (turnover / 1000).toFixed(2) + "K" :
                        turnover.toFixed(2)
            }
            property var timeStr: {
                if (!tradeTime)
                    return "-";

                if (typeof tradeTime === 'number') {
                    var date = new Date(tradeTime);
                    return Qt.formatDateTime(date, "hh:mm:ss");
                }
                return tradeTime;
            }

            // Цена
            Text {
                text: priceStr
                width: tradeView.width * root.priceField
                color: sideColor
                font.pixelSize: Theme.fontSizeSmall
                font.family: "Monospace"
            }

            // ОбЪем
            Text {
                text: volumeStr
                width: tradeView.width * root.volumeField
                color: "#EAECEF"
                font.pixelSize: Theme.fontSizeSmall
                font.family: "Monospace"
            }

            // Оборот
            Text {
                text: turnoverStr
                width: tradeView.width * root.turnoverField
                color: "#EAECEF"
                font.pixelSize: Theme.fontSizeSmall
                font.family: "Monospace"
            }

            // Время
            Text {
                text: timeStr
                width: tradeView.width * root.timeField
                color: "#707A8A"
                font.pixelSize: Theme.fontSizeSmall
                font.family: "Monospace"
            }

        } // contentItem
    } // tradeView

} // root
