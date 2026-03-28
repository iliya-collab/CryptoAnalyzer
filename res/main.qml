import QtQuick
import QtQuick.Controls

import Engine 1.0

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: "Crypto Analyzer"

    palette.window: "#2c2c2c"
    palette.windowText: "white"
    palette.button: "#3c3c3c"
    palette.buttonText: "white"

    //someProperty: Engine.TypesTrade.Spot

    // Основная панель меню
    Row {
        id: menuBar
        spacing: 2
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 8

        CustomMenuItem {
            id: generalMenu
            title: "General"
            subMenuItems: ["Settings"]
        }

        CustomMenuItem {
            id: tradeMenu
            title: "Trade"
            subMenuItems: ["Spot", "Linear", "Inverse", "Options"]

            onSubMenuItemClicked: function(item) {
                if (item === "Spot") {
                    console.log("Clicked Spot")
                    appEngine.startWork(Engine.TypesTrade.SPOT)
                }
                else if (item === "Linear") {
                    console.log("Clicked Linear")
                    appEngine.startWork(Engine.TypesTrade.LINEAR)
                }
                else if (item === "Inverse") {
                    console.log("Clicked Inverse")
                    appEngine.startWork(Engine.TypesTrade.INVERSE)
                }
                else if (item === "Option") {
                    console.log("Clicked Option")
                    appEngine.startWork(Engine.TypesTrade.OPTION)
                }
            }
        }


    }
}
