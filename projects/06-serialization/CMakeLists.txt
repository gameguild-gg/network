cmake_minimum_required(VERSION 3.20.6)

project(06-serialization LANGUAGES CXX)

# Serialization demo executable
add_executable(06-serialization-demo src/main.cpp)
target_compile_features(06-serialization-demo PRIVATE cxx_std_20)
target_link_libraries(06-serialization-demo PRIVATE Boost::endian)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(06-serialization-demo PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Tests executable with doctest (one file per section)
add_executable(06-serialization-tests
    tests/test_main.cpp
    tests/test_varint.cpp
    tests/test_bitstream.cpp
    tests/test_string.cpp
    tests/test_serialize.cpp
    tests/test_gameobject.cpp
    tests/test_protobuf.cpp
    tests/test_packet.cpp
)
target_compile_features(06-serialization-tests PRIVATE cxx_std_20)
target_link_libraries(06-serialization-tests PRIVATE Boost::endian doctest::doctest)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(06-serialization-tests PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Register with CTest
include(CTest)
add_test(NAME 06-serialization-tests
    COMMAND 06-serialization-tests
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)
