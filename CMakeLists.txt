cmake_minimum_required(VERSION 3.19)
project(app LANGUAGES CXX)

# Включаем использование RPATH для установленных бинарников
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
# Указываем искать библиотеки в папке с исполняемым файлом
set(CMAKE_INSTALL_RPATH "$ORIGIN")

# cmake --build . --target clean

#set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")
set(HEADER_DIR "${CMAKE_SOURCE_DIR}/inc")
set(RESOURCE_DIR "${CMAKE_SOURCE_DIR}/res")
set(QML_MODULES "${CMAKE_BINARY_DIR}/qml_modules")
set(QML_IMPORT_PATH "${QML_MODULES}" CACHE STRING "Extra QML Import Paths" FORCE)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Network
    Charts
    WebSockets
    Sql
    Quick
)

qt_standard_project_setup()

file(GLOB_RECURSE MY_SOURCES
    "${SOURCE_DIR}/*.cpp"
    "${SOURCE_DIR}/*.c"
)

file(GLOB_RECURSE MY_HEADERS
    "${HEADER_DIR}/*.h"
    "${HEADER_DIR}/*.hpp"
)

add_executable(${PROJECT_NAME}
    ${MY_SOURCES}
    ${MY_HEADERS}
    ${RESOURCE_DIR}/res.qrc
)

set_source_files_properties(res/Style/Theme.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

# Модуль с оформлением
qt_add_qml_module(Theme
    URI Theme
    VERSION 1.0
    RESOURCE_PREFIX "/qt/qml"
    OUTPUT_DIRECTORY "${QML_MODULES}/Theme"
    QML_FILES
        res/Style/Theme.qml
)

# Модуль с компанентами
qt_add_qml_module(Components
    URI Components
    VERSION 1.0
    RESOURCE_PREFIX "/qt/qml"
    OUTPUT_DIRECTORY "${QML_MODULES}/Components"
    QML_FILES
        res/Components/CustomButton.qml
        res/Components/CustomLabel.qml
        res/Components/CustomCheckBox.qml
        res/Components/CustomMenuBar.qml
        res/Components/CustomStatusBar.qml
        res/Components/CustomTextField.qml
        res/Components/CustomToolBar.qml
        res/Components/CustomTabBar.qml
        res/Components/SelecterTradeWidget.qml
        res/Components/OrderBookWidget.qml
        res/Components/TickerItem.qml
        res/Components/TickerWidget.qml
)

# Главный модуль
qt_add_qml_module(${PROJECT_NAME}
    URI MainApplication
    VERSION 1.0
    RESOURCE_PREFIX "/qt/qml"
    OUTPUT_DIRECTORY "${QML_MODULES}/MainApplication"
    QML_FILES
        res/Main.qml
        res/Views/ConnectAPIScreen.qml
        res/Views/TradeScreen.qml
        res/Views/SettingsWindow.qml
)

target_include_directories(${PROJECT_NAME} PRIVATE ${HEADER_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Charts
    Qt6::WebSockets
    Qt6::Sql
    Qt6::Quick
    Theme
    Components
)