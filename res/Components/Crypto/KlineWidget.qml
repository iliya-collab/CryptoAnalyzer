import QtQuick 2.15
import QtQuick.Layouts 2.15

import Engine 1.0

Item {
    id: root

    function updateKline(newKline) {
        if (newKline.confirm) {
            klineView.addCandle(newKline.timestamp, newKline.open, newKline.close, newKline.high, newKline.low)
            Engine.saveCandle(newKline)
        }
    }

    function bindKline() {
        Engine.klineUpdated.connect(root.updateKline)
    }

    function unbindKline() {
        Engine.klineUpdated.disconnect(root.updateKline)
    }

    RowLayout {
        anchors.fill: parent

        KlineChartView {
            id: klineView
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }

}
