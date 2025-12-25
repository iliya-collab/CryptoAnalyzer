cmake_minimum_required(VERSION 3.19)
project(app)

# cmake --build . --target clean

set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)


#find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets )
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Widgets
    Network
    Charts
    WebSockets
)

get_target_property(Qt6Core_includes Qt6::Core INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(Qt6Widgets_includes Qt6::Widgets INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(Qt6Network_includes Qt6::Network INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(Qt6Network_includes Qt6::Charts INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(Qt6WebSockets_includes Qt6::WebSockets INTERFACE_INCLUDE_DIRECTORIES)


qt_standard_project_setup()

set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")
set(HEADER_DIR "${CMAKE_SOURCE_DIR}/inc")

file(GLOB_RECURSE MY_SOURCES
    "${SOURCE_DIR}/*.cpp"
    "${SOURCE_DIR}/*.c"
)

file(GLOB_RECURSE MY_HEADERS
    "${HEADER_DIR}/*.h"
    "${HEADER_DIR}/*.hpp"
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_executable(${PROJECT_NAME}
    #WIN32 MACOSX_BUNDLE
    ${MY_SOURCES}
    ${MY_HEADERS}
    resources.qrc
)

#qt_add_resources(${PROJECT_NAME} "resources" "${CMAKE_SOURCE_DIR}/resources.qrc")
#set_source_files_properties(resources.qrc PROPERTIES HEADER_FILE_ONLY TRUE)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${HEADER_DIR}
    ${Qt6Core_includes}
    ${Qt6Widgets_includes}
    ${Qt6Network_includes}
    ${Qt6Charts_includes}
    ${Qt6WebSockets_includes}
)

target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE
    ${Qt6Core_includes}
    ${Qt6Widgets_includes}
    ${Qt6Network_includes}
    ${Qt6Charts_includes}
    ${Qt6WebSockets_includes}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    Qt6::Charts
    Qt6::WebSockets
)

#[[set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)]]

#include(GNUInstallDirs)

#[[if(DEFINED TYPE_PROG)
    unset(TYPE_PROG CACHE)
endif()]]

#[[install(TARGETS ${PROJECT_NAME}
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET ${PROJECT_NAME}
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})]]
