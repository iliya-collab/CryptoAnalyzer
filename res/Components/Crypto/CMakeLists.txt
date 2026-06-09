qt_add_qml_module(CryptoComponents
    URI Components.Crypto
    VERSION 1.0
    RESOURCE_PREFIX "/qt/qml"
    OUTPUT_DIRECTORY "${QML_MODULES}/Components/Crypto"
    QML_FILES
        OrderBookTableView.qml
        SelecterTradeWidget.qml
        OrderBookWidget.qml
        TickerItem.qml
        TickerWidget.qml
        KlineChartView.qml
        KlineWidget.qml

)
