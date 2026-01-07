# Network utilities library
add_library(network_utils NetworkUtils.cpp)

# Make header accessible to other targets
target_include_directories(network_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
