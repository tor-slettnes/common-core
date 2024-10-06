## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core Demo - client utilities
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(INSTALL_COMPONENT "demo-utils")

add_subdirectory(common)

if(BUILD_RTI_DDS)
  add_subdirectory(dds-tool)
endif()

if(BUILD_GRPC)
  add_subdirectory(grpc-tool)
endif()

if(BUILD_ZMQ)
  add_subdirectory(zmq-tool)
endif()


cpack_add_component(${INSTALL_COMPONENT}
  DEPENDS demo-common
  GROUP ${INSTALL_GROUP}
  DISPLAY_NAME "DEMO example - C++ utilities"
  DESCRIPTION "DEMO command-line utilitie written in C++"
)
