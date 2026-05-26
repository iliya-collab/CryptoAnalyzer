pragma Singleton
import QtQuick

QtObject {
    readonly property color windowColor: "#2C2132"
    readonly property color menuBarColor: "#302132"
    readonly property color toolBarColor: "#2B1E2D"
    readonly property color buttonColor: "#2B1E2D"
    readonly property color hoverColor: "#3F3445"
    readonly property color pressColor: "#3F3445"

    // CheckBox
    readonly property color indicatorColor: "#541961"
    readonly property int widthBox: 20

    // TextField
    readonly property color textFieldColor: "#201824"
    readonly property color hoverTextFieldColor: "#612A60"

    // ScrollBar
    readonly property color sliderColor: "#210D24" // цвет бегунка
    readonly property color scrollBarColor: "transparent" // цвет скролл-бара
    readonly property int scrollBarWidth: 5 // ширина скролл-бара
    readonly property int scrollBarHeight: 100 // высота скролл-бара
    readonly property double scrollBarMinSize: 0.05 // минимальный размер бегунка

    // Обшие параметры
    readonly property int padding: 10
    readonly property int margins: 10
    readonly property int spacing: 10
    readonly property int radius: 10
    readonly property color borderColor: "#473751"
    readonly property int borderWidth: 1

    // Шрифт
    readonly property int fontSizeHeader: 20 // размер заголовков
    readonly property int fontSizeBody: 14 // размер основного текста
    readonly property int fontSizeSmall: 11 // размер маленького текста
    readonly property string fontFamily: "Segoe UI" // шрифт
    readonly property color textColor: "#ffffff" // цвет текста
}
