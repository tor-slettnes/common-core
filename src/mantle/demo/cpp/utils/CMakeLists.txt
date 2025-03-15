## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core Demo - client utilities
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

if(BUILD_DEMO)
  set(INSTALL_COMPONENT "demo-utils")
endif()

add_subdirectory(common)

if(BUILD_DDS)
  add_subdirectory(dds-tool)
endif()

if(BUILD_GRPC)
  add_subdirectory(grpc-tool)
endif()

if(BUILD_ZMQ)
  add_subdirectory(zmq-tool)
endif()

if(INSTALL_COMPONENT)
  cpack_add_component(${INSTALL_COMPONENT}
    DEPENDS demo-settings
    GROUP ${INSTALL_GROUP}
    DISPLAY_NAME "DEMO example - C++ utilities"
    DESCRIPTION "DEMO command-line utilitie written in C++"
  )
endif()
