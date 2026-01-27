cmake_minimum_required(VERSION 3.20.6)

project(03-udp LANGUAGES CXX)

# UDP Echo Server executable
add_executable(03-udp-server src/server.cpp)
target_compile_features(03-udp-server PRIVATE cxx_std_23)
target_link_libraries(03-udp-server PRIVATE Boost::asio)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(03-udp-server PRIVATE -Wall -Wextra -Wpedantic)
endif()

# UDP Echo Client executable
add_executable(03-udp-client src/client.cpp)
target_compile_features(03-udp-client PRIVATE cxx_std_23)
target_link_libraries(03-udp-client PRIVATE Boost::asio)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(03-udp-client PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Tests executable with doctest
add_executable(03-udp-tests tests/tests.cpp)
target_compile_features(03-udp-tests PRIVATE cxx_std_23)
target_link_libraries(03-udp-tests PRIVATE Boost::asio doctest::doctest)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(03-udp-tests PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Register with CTest
include(CTest)
add_test(NAME 03-udp-tests
    COMMAND 03-udp-tests
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)
