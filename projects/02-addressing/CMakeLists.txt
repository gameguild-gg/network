cmake_minimum_required(VERSION 3.20.6)

project(02-addressing LANGUAGES CXX)

# Demo executable
add_executable(02-addressing src/main.cpp)
target_compile_features(02-addressing PRIVATE cxx_std_23)
target_link_libraries(02-addressing PRIVATE network_utils)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(02-addressing PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Tests executable with doctest
add_executable(02-addressing-tests tests/tests.cpp)
target_compile_features(02-addressing-tests PRIVATE cxx_std_23)

# Link against network utilities and doctest
target_link_libraries(02-addressing-tests PRIVATE network_utils doctest::doctest)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(02-addressing-tests PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Register with CTest
include(CTest)
add_test(NAME 02-addressing-tests
    COMMAND 02-addressing-tests
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)
