cmake_minimum_required(VERSION 3.15)

project(VoltGuard
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Main VoltGuard executable
add_executable(VoltGuard
    src/packet_interceptor/main.cpp
    src/packet_interceptor/packet_interceptor.cpp
)

target_include_directories(VoltGuard
    PRIVATE
        src/packet_interceptor
)

# --------------------------------------------------
# Testing
# --------------------------------------------------

include(CTest)
enable_testing()

# C++ Packet Interceptor test executable
add_executable(PacketInterceptorTests
    src/packet_interceptor/tests/packet_interceptor_test.cpp
    src/packet_interceptor/packet_interceptor.cpp
)

target_include_directories(PacketInterceptorTests
    PRIVATE
        src/packet_interceptor
)

add_test(
    NAME PacketInterceptorTests
    COMMAND PacketInterceptorTests
)