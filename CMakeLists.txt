cmake_minimum_required(VERSION 3.16.0)

project(AutoSort VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/fileorganizer.cpp
    src/settingsdialog.cpp
)

set(HEADERS
    src/mainwindow.h
    src/fileorganizer.h
    src/settingsdialog.h
)

qt_add_executable(AutoSort ${SOURCES} ${HEADERS})

qt_add_resources(AutoSort "resources"
    PREFIX "/"
    FILES
        "resources/icon.svg"
)

target_link_libraries(AutoSort PRIVATE Qt6::Core Qt6::Widgets)

if(WIN32)
    set_target_properties(AutoSort PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()