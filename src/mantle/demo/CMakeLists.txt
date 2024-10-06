## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core Demo
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(INSTALL_GROUP "demo")

cpack_add_component_group(${INSTALL_GROUP}
  DISPLAY_NAME "DEMO Example"
  DESCRIPTION "DEMO application example"
)

if (BUILD_PROTOBUF)
  add_subdirectory(proto)
endif()

if (BUILD_DDS)
  add_subdirectory(idl)
endif()

if (BUILD_CPP)
  add_subdirectory(cpp)
endif()

if (BUILD_PYTHON)
  add_subdirectory(python)
endif()

add_subdirectory(scripts)
add_subdirectory(settings)

cpack_add_component(demo-common
  DEPENDS demo-settings
  GROUP ${INSTALL_GROUP}
  DISPLAY_NAME "DEMO example - common components"
  DESCRIPTION "Common components for DEMO clients and servers"
)
