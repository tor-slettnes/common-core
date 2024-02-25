## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake build recipe for Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

cmake_minimum_required(VERSION 3.15)

if (NOT PROJECT_NAME)
  project(common_core
    VERSION 0.0.2)
endif()


include(${CMAKE_CURRENT_LIST_DIR}/cmake/BuildProject.cmake)

### Language components
option(BUILD_CPP        "Build/Install C++ code" ON)
option(BUILD_PYTHON     "Install Python clients/scripts" ON)

### Messaging frameworks
option(BUILD_GRPC       "Build support for gRPC Remote Procedure Calls" ON)
option(BUILD_ZMQ        "Build support for ZeroMQ" ON)
option(BUILD_HTTP       "Build support for HTTP requests, incl. REST API" ON)
option(BUILD_RTI_DDS    "Build support for RTI ConnextDDS" ON)
option(BUILD_RTI_LOGGER "Build support for RTI Distributed Logger" OFF)

### Object serialization
option(BUILD_PROTOBUF   "Build support for Google Protocol Buffers" ON)
option(BUILD_DDS        "Build support for Distributed Data Service (DDS)" ${BUILD_RTI_DDS})
option(BUILD_PYTHON_IO  "Build Python embedded environment for C++" ON)

### Build DEMO application
option(BUILD_DEMO       "Build DEMO application example" ON)

### Python components
if(BUILD_PYTHON)
  set(PYTHON_INSTALL_DIR "share/python")
endif()

### gRPC requires ProtoBuf
if(BUILD_GRPC OR BUILD_ZMQ)
  set(BUILD_PROTOBUF ON)
endif()

### Pull in CMake files to build the actual sources
add_subdirectory(src)
add_subdirectory(tests)
