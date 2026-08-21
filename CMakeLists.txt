cmake_minimum_required(VERSION 3.19)
project(EasyTrade LANGUAGES CXX)

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

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")
set(RESOURCE_DIR "${CMAKE_SOURCE_DIR}/res")
set(QML_MODULES "${CMAKE_BINARY_DIR}/qml_modules")
set(QML_IMPORT_PATH "${QML_MODULES}" CACHE STRING "Extra QML Import Paths" FORCE)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Qml
    Network
    Charts
    WebSockets
    Sql
    Quick
)

qt_standard_project_setup()

file(GLOB_RECURSE MY_SOURCES RELATIVE ${CMAKE_SOURCE_DIR}
    "${SOURCE_DIR}/*.cpp"
    "${SOURCE_DIR}/*.c"
)

file(GLOB_RECURSE MY_HEADERS RELATIVE ${CMAKE_SOURCE_DIR}
    "${SOURCE_DIR}/*.hpp"
    "${SOURCE_DIR}/*.h"
)

list(REMOVE_ITEM MY_SOURCES "src/main.cpp")

add_executable(${PROJECT_NAME}
    src/main.cpp
    ${RESOURCE_DIR}/res.qrc
)

# Регистрирует C++ классы (с QML_ELEMENT / QML_VALUE_TYPE) в системе QML.
qt_add_qml_module(EasyTradeCore
    URI Application.Core
    VERSION 1.0
    RESOURCE_PREFIX "/qt/qml"
    OUTPUT_DIRECTORY "${QML_MODULES}/Application/Core"
    SOURCES
        ${MY_SOURCES}
        ${MY_HEADERS}
)

# Линкуем бэкенд ТОЛЬКО с системными модулями и движком Qml.
target_link_libraries(EasyTradeCore PRIVATE
    Qt6::Core
    Qt6::Qml
    Qt6::Network
    Qt6::WebSockets
    Qt6::Sql
)

target_include_directories(EasyTradeCore PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/Core
)

# Делаем из исполняемого файла QML-модуль для интерфейса
qt_add_qml_module(${PROJECT_NAME}
    URI Application.UI
    VERSION 1.0
    RESOURCE_PREFIX "/qt/qml"
    OUTPUT_DIRECTORY "${QML_MODULES}/Application/UI"
    QML_FILES
        res/Main.qml
        res/Views/APIScreen.qml
        res/Views/TradeScreen.qml
        res/Views/SettingsWindow.qml
)

# Подключаем внешние UI-поддиректории
add_subdirectory(res)

# Линкуем фронтенд с графикой и Бэкендом
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::Charts
    EasyTradeCore
    Theme
    CustomComponents
    CryptoComponents
)