import QtQuick 2.15
import QtQuick.Shapes 1.15
import Theme 1.0

Item {
    id: root
    width: 60
    height: 60

    property int pingValue: 0
    property int maxPing: 300
    property int thickness: 8 // толщина линии индикатора

    onPingValueChanged: maxPing = Math.max(maxPing, pingValue)

    readonly property real angle: Math.min(pingValue / maxPing, 1.0) * 360
    readonly property color pingColor: {
        if (pingValue <= 50)
            return "#2ecc71"
        if (pingValue <= 150)
            return "#f1c40f"
        return "#e74c3c"
    }

    // Фоновая серая дуга
    Shape {
        anchors.fill: parent
        ShapePath {
            strokeColor: "#bdc3c7"
            strokeWidth: root.thickness
            fillColor: "transparent"
            capStyle: ShapePath.RoundCap

            startX: root.width / 2
            startY: root.thickness / 2
            PathArc {
                x: root.width - root.thickness / 2
                y: root.height / 2
                radiusX: root.width / 2 - root.thickness / 2
                radiusY: root.height / 2 - root.thickness / 2
                useLargeArc: false
                direction: PathArc.Clockwise
                xAxisRotation: 0
            }
        }
    }

    // Активная дуга, отражающая пинг
    Shape {
        anchors.fill: parent
        visible: root.pingValue > 0

        ShapePath {
            strokeColor: root.pingColor
            strokeWidth: root.thickness
            fillColor: "transparent"
            capStyle: ShapePath.RoundCap

            // Начало дуги сверху (угол поворота определяется математически)
            startX: root.width / 2
            startY: root.thickness / 2

            PathAngleArc {
                centerX: root.width / 2
                centerY: root.height / 2
                radiusX: root.width / 2 - root.thickness / 2
                radiusY: root.height / 2 - root.thickness / 2
                startAngle: -90 // Старт с 12 часов
                sweepAngle: root.angle
            }
        }
    }

    // Текстовое отображение значения в центре
    Text {
        anchors.centerIn: parent
        text: root.pingValue > 0 ? root.pingValue : "-"
        color: root.pingValue > 0 ? root.pingColor : "#7f8c8d"
        font.pixelSize: Theme.fontSizeSmall
    }
} // root