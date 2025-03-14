## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

add_subdirectory(settings)

if(BUILD_IDL)
  add_subdirectory(idl)
endif()

if(BUILD_PROTOBUF)
  add_subdirectory(proto)
endif()

if(BUILD_CPP)
  add_subdirectory(cpp)
endif()

# if(BUILD_PYTHON)
#   add_subdirectory(python)
# endif()
