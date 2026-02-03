cmake_minimum_required(VERSION 3.20.6)

project(04-chat LANGUAGES CXX)

# TCP Chat Server executable
add_executable(04-chat-server src/server.cpp)
target_compile_features(04-chat-server PRIVATE cxx_std_23)
target_link_libraries(04-chat-server PRIVATE Boost::asio)
if(WIN32)
    target_link_libraries(04-chat-server PRIVATE ws2_32)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(04-chat-server PRIVATE -Wall -Wextra -Wpedantic)
endif()

# TCP Chat Client executable
add_executable(04-chat-client src/client.cpp)
target_compile_features(04-chat-client PRIVATE cxx_std_23)
target_link_libraries(04-chat-client PRIVATE Boost::asio)
if(WIN32)
    target_link_libraries(04-chat-client PRIVATE ws2_32)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(04-chat-client PRIVATE -Wall -Wextra -Wpedantic)
endif()
