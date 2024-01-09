##
## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

if(USE_CPP)
  add_subdirectory(cpp)
endif()

if(USE_PYTHON)
  add_subdirectory(python)
endif()

if(USE_RTI_DDS)
  add_subdirectory(idl)
endif()

if(USE_GRPC)
  add_subdirectory(proto)
endif()

### Create interface library to encapsulate the above
set(TARGET cc_shared)
set(LIB_TYPE INTERFACE)
set(LIB_DEPS
  cc_shared_core
  cc_shared_platform
  cc_shared_ipc_api
  cc_shared_idl
  cc_shared_protodefs
  )

include(BuildLibrary)

### Install "settings/" folder as data
install(DIRECTORY settings TYPE DATA)
