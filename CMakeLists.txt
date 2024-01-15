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

### C++ Components
option(BUILD_CPP        "Build/Install C++ code" ON)
option(BUILD_CSHARP     "Build/Install C# code" OFF)
option(BUILD_PYTHON     "Install Python clients/scripts" ON)
option(BUILD_GRPC       "Build support for gRPC Remote Procedure Calls" ON)
option(BUILD_PROTOBUF   "Build support for Google Protocol Buffers" ${BUILD_GRPC})
option(BUILD_RTI_DDS    "Build support for RTI ConnextDDS" OFF)
option(BUILD_RTI_LOGGER "Build support for RTI Distributed Logger" ${BUILD_RTI_DDS})
option(BUILD_IDL        "Build Interface Definition Language types (IDL)" ${BUILD_RTI_DDS})

### C# components
if(BUILD_CSHARP)
  set(CSHARP_INSTALL_DIR "csbin")
endif()

### Python components
if(BUILD_PYTHON)
  set(PYTHON_INSTALL_DIR "share/python")
endif()

### Pull in CMake files to build the actual sources
add_subdirectory(src)
