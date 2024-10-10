## -*- cmake -*-
#===============================================================================
## @file defaults.cmake
## @brief Miscellanous build configuration
##
## Define some defaults variables to control the build outputs.  These can be
## overridden with `cmake` arguments (`-D VARIABLE=VALUE`) or with corresponding
## environment variables, and are presented as options in `cmake-gui`.
## They are also cached for future invocations; see the `build_arg()` function
## in `cmake/build_args.cmake` for details.
#===============================================================================

include(${CMAKE_CURRENT_LIST_DIR}/cmake/build_args.cmake)

#===============================================================================
## Build information

build_number(BUILD_NUMBER
  "Build number. Defaults to the previous build number incremented by one.")

build_arg(VERSION "0.3.3"
  STRING "Build version, in the format MAJOR.MINOR.PATCH")

build_arg(PRODUCT "platform"
  STRING "Product name. Used to select build configurations and components")

build_arg(PROJECT "common-core"
  STRING "Top-level project name, e.g. to look up some run-time service settings from corresponding .json filenames")

build_arg(DESCRIPTION "Cross-platform development framework for embedded microservice applications"
  STRING "Project description")

build_arg(HOME_PAGE "https://github.com/tor-slettnes/common-core/"
  STRING "Project home page.")

#===============================================================================
## Software upgrade settings (via `Upgrade` service)

build_arg(UPGRADE_PACKAGE_SUFFIX ".ccpkg"
  STRING "Filename suffix for upgrade packages; used when scanning local folders")

build_arg(UPGRADE_SCAN_URL "https://slett.net/common-core/package-index.json"
  STRING "Location of package index; used to periodically scan for upgrades")

build_arg(UPGRADE_VFS_CONTEXT "releases"
  STRING "VirtualFileSystem context in which to periodically scan for upgrade packages")

build_arg(UPGRADE_KEYRING "common-core-release.kbx"
  STRING "Filename containing public GnuPG key used to validate package signagture")

#===============================================================================
## Python code tweaks

build_arg(PYTHON_NAMESPACE "cc"
  STRING "Top-level package/namespace for Python modules")

build_arg(PYTHON_VENV "${CMAKE_SOURCE_DIR}/venv"
  PATH "Python Virtual Environment (to create executables with PyInstaller)")

build_arg(PYTHON_DEPENDENCIES "gprcio;protobuf;zmq"
  STRING "Python wheel runtime dependencies")

build_arg(PYTHON_PIP_REQUIREMENTS_FILE
  "${CMAKE_SOURCE_DIR}/src/core/python/build/venv/requirements.txt"
  FILEPATH
  "File containing required PIP-installable packages for virtual environment")

#===============================================================================
## Package generation with CPack. For information about grouping, see
## https://cmake.org/cmake/help/latest/module/CPackComponent.html#variable:CPACK_COMPONENTS_GROUPING

build_arg(CPACK_PACKAGE_NAME "cc"
  STRING "Base name for installable packages. Component or group name may be appended to this")

build_arg(CPACK_COMPONENTS_GROUPING "ONE_PER_GROUP"
  STRING "Multi-package generation"
  CHOICES "IGNORE" "ONE_PER_GROUP" "ALL_COMPONENTS_IN_ONE")

build_arg(CPACK_PACKAGE_VENDOR "Common Core Authors"
  STRING "Name of package vendor")

build_arg(CPACK_PACKAGE_CONTACT "Tor Slettnes <tor@slett.net>"
  STRING "Package contact/maintianer")

#===============================================================================
## Miscellaneous build options

### C++ options
option(BUILD_CPP         "Build/Install C++ code" ON)
option(BUILD_RAPIDJSON   "Build fast JSON parsing using RapidJSON" ON)
option(USE_STATIC_LIBS   "Use static 3rd party libraries for linking" OFF)

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

