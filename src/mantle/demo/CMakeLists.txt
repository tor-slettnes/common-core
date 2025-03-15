## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Common Core Demo
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

project("demo"
  VERSION "${VERSION}"
  DESCRIPTION "Developer reference example"
  HOMEPAGE_URL "${HOME_PAGE}"
  LANGUAGES CXX
)

if(BUILD_DEMO)
  set(INSTALL_GROUP "demo")
endif()

add_subdirectory(scripts)
add_subdirectory(settings)

### Each of the following subdirectories contain targets that may be needed by
### downstream targets, even if we don't build the DEMO application itself.

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

if(INSTALL_GROUP)
  cc_cpack_add_group(${INSTALL_GROUP}
    DISPLAY_NAME "DEMO Example"
    DESCRIPTION "DEMO application example"
    GROUP_DEPS "common"
  )
endif()
