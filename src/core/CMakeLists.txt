## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

if(BUILD_CPP)
  add_subdirectory(cpp)
endif()

if(BUILD_PYTHON)
  add_subdirectory(python)
endif()

if(BUILD_DDS)
  add_subdirectory(idl)
endif()

if(BUILD_PROTOBUF)
  add_subdirectory(proto)
endif()

### Create interface library to encapsulate the above
set(TARGET cc_core)
set(LIB_TYPE INTERFACE)
set(LIB_DEPS)

if(BUILD_CPP)
  list(APPEND LIB_DEPS
    cc_core_inner
  )
endif()

if(BUILD_DDS)
  list(APPEND LIB_DEPS
    cc_core_idl
  )
endif()

if(BUILD_PROTOBUF)
  list(APPEND LIB_DEPS
    cc_core_proto
  )
endif()


include(BuildLibrary)

### Install "settings/" folder as data
install(DIRECTORY settings TYPE DATA)
