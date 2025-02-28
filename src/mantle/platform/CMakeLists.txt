## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

project("platform"
  VERSION "${VERSION}"
  DESCRIPTION "Services to interact with the host platform"
  HOMEPAGE_URL "${HOME_PAGE}"
  LANGUAGES CXX)

set(INSTALL_GROUP "platform")

add_subdirectory(scripts)
add_subdirectory(settings)

if (BUILD_PROTOBUF)
  add_subdirectory(proto)
endif()

if (BUILD_CPP)
  add_subdirectory(cpp)
endif()

if (BUILD_PYTHON)
  add_subdirectory(python)
endif()

if(INSTALL_GROUP)
  cc_cpack_add_group(${INSTALL_GROUP}
    DISPLAY_NAME "Platform services"
    DESCRIPTION "Common platform services: System configuration, networking, file access, upgrade"
    GROUP_DEPS "common"
  )
endif()
