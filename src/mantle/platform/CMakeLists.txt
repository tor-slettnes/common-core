## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(PKG_GROUP "platform")

if (BUILD_PROTOBUF)
  add_subdirectory(proto)
endif()

if (BUILD_CPP)
  add_subdirectory(cpp)
endif()

if (BUILD_PYTHON)
  add_subdirectory(python)
endif()

add_subdirectory(scripts)
add_subdirectory(settings)


cpack_add_component_group(${PKG_GROUP}
  DISPLAY_NAME "Platform services"
  DESCRIPTION "Common platform services: System configuration, networking, file access, upgrade"
)
