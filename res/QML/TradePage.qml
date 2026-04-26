import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: root

    property alias cmbModel : selecterTrade.popupList

    Layout.fillWidth: true
    Layout.fillHeight: true
    spacing: 15

    RowLayout {
        Layout.alignment: Qt.AlignTop
        Layout.fillWidth: true
        spacing: 0

        // Устанавливаем фиксированную высоту для RowLayout
        implicitHeight: selecterTrade.implicitHeight

        SelecterTradeWidget {
            id: selecterTrade
            Layout.fillWidth: true
            implicitHeight: 50

            popupList: cmbModel
            popupWidth: 400
            popupHeight: 300
        }

        TickerWidget {
            Layout.fillWidth: true
            implicitHeight: selecterTrade.implicitHeight
        }

    } // RowLayout

    Item { Layout.fillHeight: true }

} // TradePage
