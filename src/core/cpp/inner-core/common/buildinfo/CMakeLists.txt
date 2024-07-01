## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

# Set build version, date and time
string(TIMESTAMP BUILD_DATE_UTC "%Y-%m-%d" UTC)
string(TIMESTAMP BUILD_TIME_UTC "%H:%M:%S" UTC)
string(TIMESTAMP BUILD_DATE_LOCAL "%Y-%m-%d")
string(TIMESTAMP BUILD_TIME_LOCAL "%H:%M:%S")
string(TIMESTAMP BUILD_TIMESTAMP "%s")

cmake_host_system_information(RESULT BUILD_HOST QUERY HOSTNAME)
cmake_host_system_information(RESULT BUILD_OS QUERY OS_NAME)
cmake_host_system_information(RESULT BUILD_OS_RELEASE QUERY OS_RELEASE)
cmake_host_system_information(RESULT BUILD_OS_VERSION QUERY OS_VERSION)
cmake_host_system_information(RESULT BUILD_DISTRIB_ID QUERY DISTRIB_ID)
cmake_host_system_information(RESULT BUILD_DISTRIB_NAME QUERY DISTRIB_NAME)
cmake_host_system_information(RESULT BUILD_DISTRIB_VERSION QUERY DISTRIB_VERSION)
cmake_host_system_information(RESULT BUILD_DISTRIB_VERSION_ID QUERY DISTRIB_VERSION_ID)
cmake_host_system_information(RESULT BUILD_DISTRIB_VERSION_CODENAME QUERY DISTRIB_VERSION_CODENAME)
cmake_host_system_information(RESULT BUILD_DISTRIB_PRETTY_NAME QUERY DISTRIB_PRETTY_NAME)

configure_file(buildinfo.h.in buildinfo.h)
target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
