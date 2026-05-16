cmake_minimum_required(VERSION 3.19)
project(app)

# cmake --build . --target clean

#set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)


find_package(Qt6 REQUIRED COMPONENTS
    Core
    Widgets
    Network
    Charts
    WebSockets
    Sql
    Quick
)

qt_standard_project_setup()

set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")         
set(HEADER_DIR "${CMAKE_SOURCE_DIR}/inc")       
set(RESOURCE_DIR "${CMAKE_SOURCE_DIR}/res")

file(GLOB_RECURSE MY_SOURCES
    "${SOURCE_DIR}/*.cpp"
    "${SOURCE_DIR}/*.c"
)

file(GLOB_RECURSE MY_HEADERS
    "${HEADER_DIR}/*.h"
    "${HEADER_DIR}/*.hpp"
)

set(QRC_FILES "${RESOURCE_DIR}/resources.qrc")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_executable(${PROJECT_NAME}
    ${MY_SOURCES}
    ${MY_HEADERS}
    ${QRC_FILES}
)

target_include_directories(${PROJECT_NAME} PRIVATE ${HEADER_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    Qt6::Charts
    Qt6::WebSockets
    Qt6::Sql
    Qt6::Quick
)
