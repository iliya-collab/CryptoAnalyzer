qt_add_qml_module(CryptoComponents
    URI Components.Crypto
    VERSION 1.0
    RESOURCE_PREFIX "/qt/qml"
    OUTPUT_DIRECTORY "${QML_MODULES}/Components/Crypto"
    QML_FILES
        OrderbookTableView.qml
        SelecterTradeWidget.qml
        OrderbookWidget.qml
        TickerItem.qml
        TickerWidget.qml
        KlineWidget.qml
        IndicatorMarketSentiment.qml
        CandleChart.qml
        PingIndicator.qml
        TradeWidget.qml
)
