cmake_minimum_required(VERSION 3.14)

project(bmads LANGUAGES CXX VERSION 1.0.0)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets)

set(ads_sources
    internal.h internal.cpp
    adsglobal.h adsglobal.cpp
    dropoverlay.h dropoverlay.cpp
    sectiontitlewidget.h sectiontitlewidget.cpp
    sectioncontentwidget.h sectioncontentwidget.cpp
    sectioncontent.h sectioncontent.cpp
    sectionwidget.h sectionwidget.cpp
    floatingwidget.h floatingwidget.cpp
    containerwidget.h containerwidget.cpp
)

add_library(${PROJECT_NAME} SHARED ${ads_sources} ../ads.qrc)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE
        ADS_DEBUG_PRINT
    )
elseif (${CMAKE_BUILD_TYPE} STREQUAL "Release")
endif()

target_compile_definitions(${PROJECT_NAME}
    PUBLIC
    ADS_EXPORT
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
)
