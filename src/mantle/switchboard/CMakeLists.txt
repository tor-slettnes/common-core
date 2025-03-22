## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Common Core
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

if(BUILD_SWITCHBOARD)
  set(INSTALL_GROUP "switchboard")

  set(SETTINGS_INSTALL_COMPONENT "${INSTALL_GROUP}-settings")

  if(BUILD_PYTHON)
    set(PYTHON_INSTALL_COMPONENT "${INSTALL_GROUP}-python")
  endif()

  if(BUILD_CPP)
    set(TOOLS_INSTALL_COMPONENT "${INSTALL_GROUP}-tools")
    set(SERVER_INSTALL_COMPONENT "${INSTALL_GROUP}-server")
  endif()
endif()

add_subdirectory(settings)

if(BUILD_DDS)
  add_subdirectory(idl)
endif()

if(BUILD_PROTOBUF)
  add_subdirectory(proto)
endif()

if(BUILD_PYTHON)
  add_subdirectory(python)
endif()

if(BUILD_CPP)
  add_subdirectory(cpp)
endif()

cc_cpack_add_debian_component("${SETTINGS_INSTALL_COMPONENT}"
  GROUP "${INSTALL_GROUP}"
  DEPENDS "core-settings"
  SUMMARY "Common Core - Switchboard service settings"
  DESCRIPTION "Settings for Common Core Switchboard service."
)

cc_cpack_add_debian_component("${PYTHON_INSTALL_COMPONENT}"
  GROUP "${INSTALL_GROUP}"
  DEPENDS "${SETTINGS_INSTALL_COMPONENT}" "core-python"
  SUMMARY "Common Core - Python client for Switchboard service"
  DESCRIPTION "This package contains Python client modules for the Switchboard service."
)

cc_cpack_add_debian_component("${TOOLS_INSTALL_COMPONENT}"
  GROUP "${INSTALL_GROUP}"
  DEPENDS "${SETTINGS_INSTALL_COMPONENT}"
  SUMMARY "Common Core - Switchboard service command line tools"
  DESCRIPTION "Command line tools to communiate with the Switchboard service via gRPC and/or DDS."
)

cc_cpack_add_debian_component("${SERVER_INSTALL_COMPONENT}"
  GROUP "${INSTALL_GROUP}"
  DEPENDS "${SETTINGS_INSTALL_COMPONENT}"
  SUMMARY "Common Core - Switchboard server"
  DESCRIPTION "Manage common core runtime dependencies"
)

cc_cpack_add_group("${INSTALL_GROUP}"
  GROUP_DEPS "core"
  SUMMARY "Unified observation, control, and dependency management. "
  DESCRIPTION "Switchboard provides unified observation, control, and"
  "dependency management of runtime conditions in a distributed system."
  "."
  "It is effectively a collection of binary _switches_, each of which"
  "represents a binary physical or logical state, along with corresponding"
  "dependency rules, activation/deactivation triggers, and error handling."
  "."
  "Think of it as a unified virtual control and observation panel."
)
