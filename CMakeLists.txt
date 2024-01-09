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
option(USE_CPP "Build/Install C++ code" ON)

### C# components
#option(BUILD_CSHARP "Build/Install C# code" ${WIN32})
option(USE_CSHARP "Build/Install C# code" OFF)
set(CSHARP_INSTALL_DIR "csbin")

### Python components
option(USE_PYTHON "Install Python clients/scripts" ON)
set(PYTHON_INSTALL_DIR "share/python")

### DDS
option(USE_RTI_DDS "Build support for Distributed Data Service (DDS)" ON)
option(USE_RTI_LOGGER "Build support for RTI Distributed Logger" ON)

### gRPC
option(USE_GRPC "Build support for gRPC Remote Procedure Calls" ON)

### Pull in CMake files to build the actual sources
add_subdirectory(src)
