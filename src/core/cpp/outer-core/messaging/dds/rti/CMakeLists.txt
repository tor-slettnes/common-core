## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library. This can be used as a dependency for downstream libraries/executables.
set(TARGET cc_core_messaging_dds_rti)

### Library dependencies.
set(LIB_DEPS
  cc_core_messaging_base
  cc_core_io_idl
  cc_core_common
)

set(SOURCES
  dds-endpoint.c++
  dds-publisher.c++
  dds-subscriber.c++
  dds-logsink.c++
#  dds-options-server.c++
  dds-server.c++
)

### Logging via DDS
if(BUILD_RTI_LOGGER)
  list(APPEND SOURCES
    rti-dl-logsink.c++
  )
endif()


### Recepies to build the library
cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)

## RTI DDS
include(rti-connext-dds)
target_link_libraries("${TARGET}" PUBLIC RTIConnextDDS::cpp2_api)

if(BUILD_RTI_LOGGER)
  string(TOLOWER "$<CONFIG>" buildconfig)
  if("${buildconfig}" STREQUAL "debug")
    target_link_libraries("${TARGET}" PUBLIC "rtidlcppzd" "rtidlczd")
  else()
    target_link_libraries("${TARGET}" PUBLIC "rtidlcppz" "rtidlcz")
  endif()
  target_link_libraries("${TARGET}" PUBLIC RTIConnextDDS::cpp_api)
  target_compile_definitions("${TARGET}" PUBLIC BUILD_RTI_LOGGER)
endif()

### C++17 required for variant data type, parameter packs, std::filesystem
target_compile_features("${TARGET}" PUBLIC cxx_std_17)
target_compile_definitions("${TARGET}" PUBLIC USE_DDS=1 USE_RTI_DDS=1)
