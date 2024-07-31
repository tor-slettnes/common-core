## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(PKG_GROUP "platform")

cpack_add_component_group(${PKG_GROUP}
  DISPLAY_NAME "Platform services"
  DESCRIPTION "Common platform services: System configuration, networking, file access, upgrade"
)

add_subdirectory(proto)
add_subdirectory(cpp)
add_subdirectory(python)
add_subdirectory(scripts)
add_subdirectory(settings)
