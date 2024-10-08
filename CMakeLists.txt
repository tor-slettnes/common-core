## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake build recipe for Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

cmake_minimum_required(VERSION 3.15)

### C++ options
option(BUILD_CPP         "Build/Install C++ code" ON)
option(BUILD_RAPIDJSON   "Build fast JSON parsing using RapidJSON" ON)
option(USE_STATIC_LIBS   "Use static libraries for linking" OFF)

### Python options
option(BUILD_PYTHON            "Install Python clients/scripts" ON)
option(BUILD_PYTHON_EXECUTABLE "Build and install Python executables with PyInstaller" ON)
option(BUILD_PYTHON_WHEELS     "Build and install Python distributions (`.whl`)" ON)
option(INSTALL_PYTHON_MODULES  "Install/package Python source modules" ON)

### Messaging frameworks
option(BUILD_GRPC        "Build support for gRPC Remote Procedure Calls" ON)
option(BUILD_ZMQ         "Build support for ZeroMQ" ON)
option(BUILD_HTTP        "Build support for HTTP requests, incl. REST API" ON)
option(BUILD_RTI_DDS     "Build support for RTI ConnextDDS" OFF)
option(BUILD_RTI_LOGGER  "Build support for RTI Distributed Logger" OFF)

### Object serialization
option(BUILD_PROTOBUF    "Build support for Google Protocol Buffers" ON)
option(BUILD_DDS         "Build support for Distributed Data Service (DDS)" ${BUILD_RTI_DDS})
option(BUILD_PYTHON_IO   "Build Python embedded environment for C++" ON)
option(BUILD_DBUS        "Build support for D-Bus" ${LINUX})
option(BUILD_DNSSD_AVAHI "Build suport for DNS-SD via Avahi" ON)

### Build DEMO application
option(BUILD_DEMO        "Build DEMO application example" ON)

### Build tests
option(BUILD_TESTING     "Build testing modules" ON)

### Enable SystemD service integration
option(ENABLE_SERVICES   "Enable SystemD service units" ON)

### gRPC requires ProtoBuf
if(BUILD_GRPC OR BUILD_ZMQ)
  set(BUILD_PROTOBUF ON)
endif()

### Method to obtain build settings, with defaults from `defaults.json`
include(${CMAKE_CURRENT_LIST_DIR}/cmake/build_args.cmake)

### Global build configuration
get_build_arg(PROJECT
  SETTING "project"
  DOCSTRING "Top-level project name. Used e.g. to look up some run-time service settings from corresponding .json filenames"
  DEFAULT "common-core")

get_build_arg(PRODUCT
  SETTING "product"
  DOCSTRING "Product name"
  DEFAULT "platform")

get_build_arg(VERSION
  SETTING "version"
  DOCSTRING "Build version, in the format MAJOR.MINOR.PATCH"
  REQUIRED)

get_build_arg(DESCRIPTION
  SETTING "description"
  DOCSTRING "Project description"
)

get_build_arg(HOME_PAGE
  SETTING "home page"
  DOCSTRING "Home page for this project, if any")

get_build_number(BUILD_NUMBER)

project("${PROJECT}"
  VERSION "${VERSION}"
  DESCRIPTION "${DESCRIPTION}"
  HOMEPAGE_URL "${HOME_PAGE}"
  LANGUAGES CXX
)

message(STATUS
  "Building project \"${PROJECT_NAME}\" version ${PROJECT_VERSION}, build #${BUILD_NUMBER}")

### Invoke common project settings
include(${CMAKE_CURRENT_LIST_DIR}/cmake/BuildProject.cmake)

### Pull in CMake files to build the actual sources
add_subdirectory(thirdparty)
add_subdirectory(src)
#add_subdirectory(tests)
