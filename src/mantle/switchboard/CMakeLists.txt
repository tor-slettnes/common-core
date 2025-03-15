## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

if(BUILD_SWITCHBOARD)
  set(INSTALL_GROUP "switchboard")
endif()

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

if(INSTALL_GROUP)
  cc_cpack_add_group(${INSTALL_GROUP}
    GROUP_DEPS "common"
    DISPLAY_NAME "Unified observation, control, and dependency management. "
    DESCRIPTION "Switchboard provides unified observation, control, and"
    "dependency management of runtime conditions in a distributed system."
    "."
    "It is effectively a collection of binary _switches_, each of which"
    "represents a binary physical or logical state, along with corresponding"
    "dependency rules, activation/deactivation triggers, and error handling."
    "."
    "Think of it as a unified virtual control and observation panel."
  )
endif()
