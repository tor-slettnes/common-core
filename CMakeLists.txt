## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake build recipe for Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

cmake_minimum_required(VERSION 3.15)

if (NOT PROJECT_NAME)
  project(common_core
    VERSION 0.0.1)
endif()


include(${CMAKE_CURRENT_LIST_DIR}/cmake/BuildProject.cmake)

### Language components
option(BUILD_CPP        "Build/Install C++ code" ON)
option(BUILD_CSHARP     "Build/Install C# code" OFF)
option(BUILD_PYTHON     "Install Python clients/scripts" ON)

### Object serialization
option(BUILD_PYPICKLE   "Build support for Python serialized data in C++" ON)
option(BUILD_PROTOBUF   "Build support for Google Protocol Buffers" ON)

### Messaging frameworks
option(BUILD_ZMQ        "Build support for ZeroMQ" ON)
option(BUILD_GRPC       "Build support for gRPC Remote Procedure Calls" ON)
option(BUILD_RTI_DDS    "Build support for RTI ConnextDDS" ON)
option(BUILD_RTI_LOGGER "Build support for RTI Distributed Logger" OFF)
option(BUILD_IDL        "Build Interface Definition Language types (IDL)" ${BUILD_RTI_DDS})

### C# components
if(BUILD_CSHARP)
  set(CSHARP_INSTALL_DIR "csbin")
endif()

### Python components
if(BUILD_PYTHON)
  set(PYTHON_INSTALL_DIR "share/python")
endif()

### gRPC requires ProtoBuf
if(BUILD_GRPC)
  set(BUILD_PROTOBUF ON)
endif()

### DDS requires IDL
if(BUILD_RTI_DDS)
  set(BUILD_IDL ON)
endif()

### Pull in CMake files to build the actual sources
add_subdirectory(src)
