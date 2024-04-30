cmake_minimum_required(VERSION 3.14)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(example LANGUAGES CXX VERSION 1.0.0)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets)

add_executable(${PROJECT_NAME} WIN32
    main.cpp
    mainwindow.h mainwindow.cpp
    mainwindow.ui
    icontitlewidget.h icontitlewidget.cpp
)

target_compile_definitions(${PROJECT_NAME}
    PUBLIC
    ADS_IMPORT
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    bmads
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
)
