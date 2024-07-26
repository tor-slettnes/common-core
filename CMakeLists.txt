## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake build recipe for Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

cmake_minimum_required(VERSION 3.15)

set(VERSION_FILE "${CMAKE_SOURCE_DIR}/.version")

if(CMAKE_PROJECT_VERSION)
  set(VERSION "${CMAKE_PROJECT_VERSION}")
  message(STATUS "Using inherited or cached version: ${VERSION}")

elseif(NOT "$ENV{VERSION}" STREQUAL "")
  set(VERSION "$ENV{VERSION}")
  message(STATUS "Setting new version: ${VERSION}")
  file(WRITE "${VERSION_FILE}" "${VERSION}\n")

elseif(EXISTS "${VERSION_FILE}")
  file(STRINGS "${VERSION_FILE}" VERSION)
  message(STATUS "Using saved version: ${VERSION}")

else()
  set(VERSION 0.0.0)
  message(WARNING
    "No VERSION argument was supplied and none found in ${VERSION_FILE}. "
    "Falling back to ${VERSION}")
endif()

project(common-core
  VERSION "${VERSION}"
  DESCRIPTION "Cross-platform development framework for embedded microservice applications"
  HOMEPAGE_URL "https://github.com/tor-slettnes/common-core/"
  LANGUAGES CXX
)

### Language components
option(BUILD_CPP        "Build/Install C++ code" ON)
option(BUILD_PYTHON     "Install Python clients/scripts" ON)

### Optional core components
option(BUILD_RAPIDJSON  "Build fast JSON parsing using RapidJSON" ON)

### Messaging frameworks
option(BUILD_GRPC       "Build support for gRPC Remote Procedure Calls" ON)
option(BUILD_ZMQ        "Build support for ZeroMQ" ON)
option(BUILD_HTTP       "Build support for HTTP requests, incl. REST API" ON)
option(BUILD_RTI_DDS    "Build support for RTI ConnextDDS" OFF)
option(BUILD_RTI_LOGGER "Build support for RTI Distributed Logger" OFF)

### Object serialization
option(BUILD_PROTOBUF   "Build support for Google Protocol Buffers" ON)
option(BUILD_DDS        "Build support for Distributed Data Service (DDS)" ${BUILD_RTI_DDS})
option(BUILD_PYTHON_IO  "Build Python embedded environment for C++" ON)
option(BUILD_DBUS       "Build support for D-Bus" ${LINUX})

### Build DEMO application
option(BUILD_DEMO       "Build DEMO application example" ON)

### Build tests
option(BUILD_TESTING    "Build testing modules" ON)

### Use static linking
option(USE_STATIC_LIBS "Use static libraries for linking" OFF)

### Enable SystemD service integration
option(ENABLE_SERVICES "Enable SystemD service units" ON)

### gRPC requires ProtoBuf
if(BUILD_GRPC OR BUILD_ZMQ)
  set(BUILD_PROTOBUF ON)
endif()

### Invoke common project settings
include(${CMAKE_CURRENT_LIST_DIR}/cmake/BuildProject.cmake)

### Pull in CMake files to build the actual sources
add_subdirectory(thirdparty)
add_subdirectory(src)
add_subdirectory(tests)
