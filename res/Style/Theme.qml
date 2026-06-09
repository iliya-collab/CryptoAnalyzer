pragma Singleton
import QtQuick

QtObject {
    readonly property color windowColor: "#2e2e2e"
    readonly property color menuBarColor: "#1f1f1f"
    readonly property color toolBarColor: "#2e2e2e"
    readonly property color buttonColor: "#1c1c1c"
    readonly property color hoverColor: "#a6a6a6"
    readonly property color pressColor: "#a6a6a6"

    // CheckBox
    readonly property color indicatorColor: "#ffffff"
    readonly property int widthBox: 20

    // TextField
    readonly property color textFieldColor: "#1c1c1c"
    readonly property color hoverTextFieldColor: "#d4d4d4"

    // ScrollBar
    readonly property color sliderColor: "#2e2e2e" // цвет бегунка
    readonly property color scrollBarColor: "transparent" // цвет скролл-бара
    readonly property int scrollBarWidth: 5 // ширина скролл-бара
    readonly property int scrollBarHeight: 100 // высота скролл-бара
    readonly property double scrollBarMinSize: 0.05 // минимальный размер бегунка

    // Обшие параметры
    readonly property int padding: 10
    readonly property int margins: 10
    readonly property int spacing: 10
    readonly property int radius: 10
    readonly property color borderColor: "#adadad"
    readonly property int borderWidth: 1

    // Шрифт
    readonly property int fontSizeHeader: 20 // размер заголовков
    readonly property int fontSizeBody: 14 // размер основного текста
    readonly property int fontSizeSmall: 11 // размер маленького текста
    readonly property string fontFamily: "Segoe UI" // шрифт
    readonly property color textColor: "#ffffff" // цвет текста
}
