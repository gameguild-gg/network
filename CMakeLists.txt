# set the minimum cmake version to generate projects.
cmake_minimum_required(VERSION 3.20.6)

project(
        gg-network
        LANGUAGES CXX
        VERSION 0.0.1 # todo: get this version from git tag
)

# ---- Add dependencies via CPM ----
# see https://github.com/TheLartians/CPM.cmake for more info
include(cmake/get_cpm.cmake)

# Set C++23 globally
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# include external libs
message("Git clonning doctest...")
CPMAddPackage("gh:doctest/doctest@2.4.12")

# include formating
message("Git clonning Format...")
CPMAddPackage("gh:TheLartians/Format.cmake@1.8.3")

# include boost
message("Downloading Boost... Do not stop the process, it may take a while...")
CPMAddPackage(
  NAME Boost
  VERSION 1.89.0
  URL https://github.com/boostorg/boost/releases/download/boost-1.89.0/boost-1.89.0-cmake.tar.xz
  URL_HASH SHA256=67acec02d0d118b5de9eb441f5fb707b3a1cdd884be00ca24b9a73c995511f74
  OPTIONS "BOOST_ENABLE_CMAKE ON" "BOOST_SKIP_INSTALL_RULES ON" # Set `OFF` for installation
          "BUILD_SHARED_LIBS OFF" "BOOST_INCLUDE_LIBRARIES container\\\;asio" # Note the escapes!
)

# Handle libpcap dependency cross-platform
if(WIN32)
  # Windows: use npcap-sdk
  message("Windows detected: Using npcap-sdk...")
  CPMAddPackage(
    NAME NpcapSdk
    VERSION 1.13
    URL https://github.com/nmap/npcap/releases/download/v1.13/npcap-sdk-1.13.zip
    DOWNLOAD_ONLY TRUE
  )
  
  # Create PCAP::PCAP target pointing to npcap-sdk
  add_library(PCAP::PCAP INTERFACE IMPORTED GLOBAL)
  target_include_directories(PCAP::PCAP INTERFACE "${NpcapSdk_SOURCE_DIR}/Include")
  
  # Determine correct architecture subdirectory
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "ARM64|aarch64")
    set(NPCAP_LIB_DIR "${NpcapSdk_SOURCE_DIR}/Lib/arm64")
  elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "ARM|armv7")
    set(NPCAP_LIB_DIR "${NpcapSdk_SOURCE_DIR}/Lib/arm")
  elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(NPCAP_LIB_DIR "${NpcapSdk_SOURCE_DIR}/Lib/x64")
  else()
    set(NPCAP_LIB_DIR "${NpcapSdk_SOURCE_DIR}/Lib")
  endif()
  
  target_link_libraries(PCAP::PCAP INTERFACE "${NPCAP_LIB_DIR}/wpcap.lib" "${NPCAP_LIB_DIR}/packet.lib")
else()
  # Unix systems (macOS, Linux): use pkg-config to find system libpcap
  message("Unix-based system detected: Using pkg-config to find libpcap...")
  find_package(PkgConfig QUIET)
  
  if(PkgConfig_FOUND)
    pkg_check_modules(PCAP libpcap)
    
    if(PCAP_FOUND)
      message("Found system libpcap via pkg-config")
      # Create PCAP::PCAP target from pkg-config results
      add_library(PCAP::PCAP INTERFACE IMPORTED GLOBAL)
      target_include_directories(PCAP::PCAP INTERFACE ${PCAP_INCLUDE_DIRS})
      target_link_libraries(PCAP::PCAP INTERFACE ${PCAP_LIBRARIES})
      target_compile_options(PCAP::PCAP INTERFACE ${PCAP_CFLAGS_OTHER})
    else()
      message(FATAL_ERROR "libpcap not found. Install it with: apt-get install libpcap-dev (Linux) or brew install libpcap (macOS)")
    endif()
  else()
    message(FATAL_ERROR "pkg-config not found. Install it with: apt-get install pkg-config (Linux) or brew install pkg-config (macOS)")
  endif()
endif()

# include PcapPlusPlus
message("Downloading PcapPlusPlus... Do not stop the process, it may take a while...")
CPMAddPackage(
  NAME PcapPlusPlus
  VERSION 25.05
  URL https://github.com/seladb/PcapPlusPlus/archive/refs/tags/v25.05.tar.gz
  OPTIONS 
    "PCAPPP_BUILD_EXAMPLES OFF" 
    "PCAPPP_BUILD_TESTS OFF"
    "TREAT_WARNINGS_AS_ERRORS OFF"
)

# Enable testing
enable_testing()

add_subdirectory(lib)
add_subdirectory(projects)