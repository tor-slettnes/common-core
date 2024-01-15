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

if(BUILD_IDL)
  add_subdirectory(idl)
endif()

if(BUILD_PROTOBUF)
  add_subdirectory(proto)
endif()

### Create interface library to encapsulate the above
set(TARGET cc_shared)
set(LIB_TYPE INTERFACE)
set(LIB_DEPS
  cc_shared_core
  cc_shared_platform
  cc_shared_ipc_api
)

if(BUILD_RTI_DDS)
  list(APPEND LIB_DEPS
    cc_shared_idl
  )
endif()

if(BUILD_PROTOBUF)
  list(APPEND LIB_DEPS
    cc_shared_protodefs
  )
endif()


include(BuildLibrary)

### Install "settings/" folder as data
install(DIRECTORY settings TYPE DATA)
