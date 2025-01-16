## -*- cmake -*-
#===============================================================================
## @file defaults.cmake
## @brief Miscellanous build configuration
##
## Define some defaults variables to control the build outputs.  These can be
## overridden with `cmake` arguments (`-D VARIABLE=VALUE`), and are presented as
## options in `cmake-gui`. They are also cached for future invocations.
##
## To make local persistent changes, do not edit this file, since you may
## unintentionally commit these to Git.  Instead, put your customizations in
## your own `local.cmake` file in this folder; as that file is listed in
## `.gitignore` it won't be committed.  For your custom setting to take effect
## you may need to use a plain `set()` invocation without the CACHE option, or
## add the `FORCE` option, or else clean the CMake cache before rebuilding.
#===============================================================================

#===============================================================================
## Build information

set(VERSION "0.3.3"
  CACHE STRING "Build version, in the format MAJOR.MINOR.PATCH")

set(PRODUCT "platform"
  CACHE STRING "Product name. Used to select build configurations and components")

set(PROJECT "common-core"
  CACHE STRING "Top-level project name. Used to look up some run-time service settings")

set(DESCRIPTION "Cross-platform development framework for embedded microservice applications"
  CACHE STRING "Project description")

set(HOME_PAGE "https://github.com/tor-slettnes/common-core/"
  CACHE STRING "Project home page.")


if(NOT BUILD_NUMBER)
  ### No BUILD_NUMBER argument was provided. Let's check the BUILD_NUMBER
  if(DEFINED ENV{BUILD_NUMBER})
    set(BUILD_NUMBER $ENV{BUILD_NUMBER})
  else()
    math(EXPR BUILD_NUMBER "${LAST_BUILD}+1")
  endif()
endif()

set(LAST_BUILD "${BUILD_NUMBER}" CACHE INTERNAL "..." FORCE)


#===============================================================================
## Software upgrade settings (via `Upgrade` service)

set(UPGRADE_PACKAGE_SUFFIX ".ccpkg"
  CACHE STRING "Filename suffix for upgrade packages; used when scanning local folders")

set(UPGRADE_SCAN_URL "https://slett.net/common-core/package-index.json"
  CACHE STRING "Location of package index; used to periodically scan for upgrades")

set(UPGRADE_VFS_CONTEXT "releases"
  CACHE STRING "VirtualFileSystem context in which to periodically scan for upgrade packages")

set(UPGRADE_KEYRING "common-core-release.kbx"
  CACHE STRING "Filename containing public GnuPG key used to validate package signagture")

#===============================================================================
## Python code tweaks

set(PYTHON_NAMESPACE "cc"
  CACHE STRING "Top-level package/namespace for Python modules")

set(PYTHON_VENV "${CMAKE_SOURCE_DIR}/venv"
  CACHE PATH "Python Virtual Environment (to create executables with PyInstaller)")

set(PYTHON_DEPENDENCIES "gprcio;protobuf;zmq"
  CACHE STRING "Python wheel runtime dependencies")

set(PYTHON_PIP_REQUIREMENTS_FILE
  "${CMAKE_CURRENT_LIST_DIR}/src/core/python/buildtools/virtualenv/requirements.txt"
  CACHE FILEPATH
  "File containing required PIP-installable packages for virtual environment")

#===============================================================================
## Package generation with CPack. For information about grouping, see

option(PACKAGE_SPLIT_BY_COMPONENT
  "Create one installation package per component"
  FALSE)

option(PACKAGE_SPLIT_BY_GROUP
  "Create one installation package per component group"
  TRUE)

if(PACKAGE_NAME_PREFIX)
  set(force FORCE)
else()
  set(force)
  set(PACKAGE_NAME_PREFIX "cc")
endif()

set(CPACK_PACKAGE_NAME ${PACKAGE_NAME_PREFIX}
  CACHE STRING "Base name for installable packages. Component or group name may be appended to this"
  ${force})

set(CPACK_PACKAGE_VENDOR "Common Core Authors"
  CACHE STRING "Name of package vendor")

set(CPACK_PACKAGE_CONTACT "Tor Slettnes <tor@slett.net>"
  CACHE STRING "Package contact/maintainer")

#===============================================================================
## Miscellaneous build options

### C++ options
option(BUILD_CPP         "Build/Install C++ code" ON)
option(BUILD_RAPIDJSON   "Build fast JSON parsing using RapidJSON" ON)
option(BUILD_YAML        "Build support for YAML parsing " ON)
option(USE_STATIC_LIBS   "Use static 3rd party libraries for linking" OFF)

### Python options
option(BUILD_PYTHON            "Install Python clients/scripts" ON)
option(BUILD_PYTHON_EXECUTABLE "Build and install Python executables with PyInstaller" ON)
option(BUILD_PYTHON_WHEELS     "Build and install Python distributions (`.whl`)" ON)
option(INSTALL_PYTHON_MODULES  "Install/package Python source modules" ON)

### Messaging frameworks
option(BUILD_KAFKA       "Build support for Apache Kafka" ON)
option(BUILD_GRPC        "Build support for gRPC Remote Procedure Calls" ON)
option(BUILD_ZMQ         "Build support for ZeroMQ" ON)
option(BUILD_HTTP        "Build support for HTTP requests, incl. REST API" ON)
option(BUILD_RTI_DDS     "Build support for RTI ConnextDDS" OFF)
option(BUILD_RTI_LOGGER  "Build support for RTI Distributed Logger" OFF)

### Object serialization
option(BUILD_SQLITE3     "Build support for SQLite3 DB" ON)
option(BUILD_AVRO        "Build support for Apache AVRO" ${BUILD_KAFKA})
option(BUILD_PROTOBUF    "Build support for Google Protocol Buffers" ON)
option(BUILD_DDS         "Build support for Distributed Data Service (DDS)" ${BUILD_RTI_DDS})
option(BUILD_PYTHON_IO   "Build Python embedded environment for C++" ON)
option(BUILD_DBUS        "Build support for D-Bus" ${LINUX})
option(BUILD_DNSSD_AVAHI "Build suport for DNS-SD via Avahi" ON)

### Build tests
option(BUILD_TESTING     "Build testing modules" ON)

### What applications to build/install.
option(INCLUDE_SHARED    "Install shared artifacts (e.g. common settings)" ON)
option(INCLUDE_DEMO      "Build/install DEMO application example" ${INCLUDE_SHARED})
option(INCLUDE_PLATFORM  "Build/install Platform services" ${INCLUDE_SHARED})
option(INCLUDE_LOGGER    "Build/install Logger service" ${INCLUDE_SHARED})

### Enable SystemD service integration
option(ENABLE_SERVICES   "Enable SystemD service units" ON)

### gRPC requires ProtoBuf
if(BUILD_GRPC OR BUILD_ZMQ)
  set(BUILD_PROTOBUF ON)
endif()
