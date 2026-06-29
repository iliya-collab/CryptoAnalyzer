qt_add_qml_module(CustomComponents
    URI Components.Custom
    VERSION 1.0
    RESOURCE_PREFIX "/qt/qml"
    OUTPUT_DIRECTORY "${QML_MODULES}/Components/Custom"
    QML_FILES
        CustomButton.qml
        CustomLabel.qml
        CustomText.qml
        CustomCheckBox.qml
        CustomMenuBar.qml
        CustomStatusBar.qml
        CustomTextField.qml
        CustomToolBar.qml
        CustomTabBar.qml
        CustomCandleChart.qml

)
