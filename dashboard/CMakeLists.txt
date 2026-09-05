cmake_minimum_required(VERSION 3.16)

project(VoltGuardDashboard
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_executable(VoltGuardDashboard
    main.cpp
    dashboard.cpp
    dashboard.h
    trend_widget.cpp
    trend_widget.h
)

target_link_libraries(VoltGuardDashboard
    PRIVATE
        Qt6::Widgets
)