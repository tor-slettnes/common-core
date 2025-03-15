## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Common Core static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

# Set build version, date and time
cmake_host_system_information(RESULT BUILD_HOST QUERY HOSTNAME)
cmake_host_system_information(RESULT BUILD_OS QUERY OS_NAME)
cmake_host_system_information(RESULT BUILD_OS_RELEASE QUERY OS_RELEASE)
cmake_host_system_information(RESULT BUILD_OS_VERSION QUERY OS_VERSION)
cmake_host_system_information(RESULT BUILD_DISTRIB_ID QUERY DISTRIB_ID)
cmake_host_system_information(RESULT BUILD_DISTRIB_NAME QUERY DISTRIB_NAME)
cmake_host_system_information(RESULT BUILD_DISTRIB_VERSION QUERY DISTRIB_VERSION)
cmake_host_system_information(RESULT BUILD_DISTRIB_VERSION_ID QUERY DISTRIB_VERSION_ID)
cmake_host_system_information(RESULT BUILD_DISTRIB_VERSION_CODE QUERY DISTRIB_VERSION_CODENAME)
cmake_host_system_information(RESULT BUILD_DISTRIB_PRETTY_NAME QUERY DISTRIB_PRETTY_NAME)

cc_get_value_or_default(VERSION_MAJOR CMAKE_PROJECT_VERSION_MAJOR 0)
cc_get_value_or_default(VERSION_MINOR CMAKE_PROJECT_VERSION_MINOR 0)
cc_get_value_or_default(VERSION_PATCH CMAKE_PROJECT_VERSION_PATCH 0)
cc_get_value_or_default(VERSION_TWEAK CMAKE_PROJECT_VERSION_TWEAK 0)

configure_file(buildinfo.c++.in buildinfo.c++)

target_sources(${TARGET} PRIVATE
  ${CMAKE_CURRENT_BINARY_DIR}/buildinfo.c++)

target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
