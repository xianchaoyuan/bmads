cmake_minimum_required(VERSION 3.14)

project(bmads LANGUAGES CXX VERSION 1.0.0)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets)

set(ads_sources
    adsglobal.h adsglobal.cpp
)

add_library(${PROJECT_NAME} SHARED ${ads_sources})

target_compile_definitions(${PROJECT_NAME}
    PUBLIC
    ADS_EXPORT
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
)
