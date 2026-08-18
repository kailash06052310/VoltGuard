cmake_minimum_required(VERSION 3.15)

project(VoltGuard
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(VoltGuard
    src/packet_interceptor/main.cpp
    src/packet_interceptor/packet_interceptor.cpp
)

target_include_directories(VoltGuard
    PRIVATE
        src/packet_interceptor
)