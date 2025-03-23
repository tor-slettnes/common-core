## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Common Core
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(COMMON_DESRIPTION
  "Switchboard provides unified observation, control, and dependency"
  "management of runtime conditions in a distributed system."
  "."
  "It is effectively a connected graph of binary switches, each of which"
  "represents a physical or logical condition, along with corresponding"
  "dependency rules, activation/deactivation triggers, and error handling."
  "."
  "Think of it as a unified virtual control and observation panel."
  "It is particularly useful for bringup and shutdown of components"
  "and associated services in a distributed system."
)

set(INSTALL_GROUP "switchboard")

set(SETTINGS_INSTALL_COMPONENT "${INSTALL_GROUP}-settings")

if(BUILD_PYTHON)
  set(PYTHON_INSTALL_COMPONENT "${INSTALL_GROUP}-python")
endif()

if(BUILD_CPP)
  set(TOOLS_INSTALL_COMPONENT "${INSTALL_GROUP}-tools")
  set(SERVER_INSTALL_COMPONENT "${INSTALL_GROUP}-server")
endif()

set(COMPLETE_INSTALL_COMPONENT "${INSTALL_GROUP}")
set(COMPLETE_INSTALL_DEPENDENCIES
  ${SETTINGS_INSTALL_COMPONENT}
  ${PYTHON_INSTALL_COMPONENT}
  ${TOOLS_INSTALL_COMPONENT}
  ${SERVER_INSTALL_COMPONENT}
)

#-------------------------------------------------------------------------------

cc_cpack_add_debian_component("${SETTINGS_INSTALL_COMPONENT}"
  GROUP "${INSTALL_GROUP}"
  DEPENDS "core-settings"
  LICENSE ${LICENSE_FILE}
  SUMMARY "Common Core Switchboard - settings"
  DESCRIPTION ${COMMON_DESCRIPTION}
  "."
  "This package contains settings files."
)

cc_cpack_add_debian_component("${PYTHON_INSTALL_COMPONENT}"
  GROUP "${INSTALL_GROUP}"
  DEPENDS "${SETTINGS_INSTALL_COMPONENT}" "core-python"
  LICENSE ${LICENSE_FILE}
  SUMMARY "Common Core Switchboard - Python client package"
  DESCRIPTION ${COMMON_DESCRIPTION}
  "."
  "This package contains Python client modules."
)

cc_cpack_add_debian_component("${TOOLS_INSTALL_COMPONENT}"
  GROUP "${INSTALL_GROUP}"
  DEPENDS "${SETTINGS_INSTALL_COMPONENT}"
  LICENSE ${LICENSE_FILE}
  SUMMARY "Common Core Switchboard - command line tools"
  DESCRIPTION ${COMMON_DESCRIPTION}
  "."
  "This package contains command-line client tools written in C++."
)

cc_cpack_add_debian_component("${SERVER_INSTALL_COMPONENT}"
  GROUP "${INSTALL_GROUP}"
  DEPENDS "${SETTINGS_INSTALL_COMPONENT}"
  LICENSE ${LICENSE_FILE}
  SUMMARY "Common Core Switchboard - server"
  DESCRIPTION ${COMMON_DESCRIPTION}
  "."
  "This package contains the server daemon, written in C++."
)

cc_cpack_add_debian_component("${COMPLETE_INSTALL_COMPONENT}"
  GROUP "${INSTALL_GROUP}"
  DEPENDS ${COMPLETE_INSTALL_DEPENDENCIES}
  LICENSE ${LICENSE_FILE}
  SUMMARY "Common Core Switchboard - complete"
  DESCRIPTION ${COMMON_DESCRIPTION}
  "."
  "This meta-package depends on and will thus install all Switchboard packages."
  "You may choose to select specific component packages instead."
)

cc_cpack_add_group("${INSTALL_GROUP}"
  GROUP_DEPS "core"
  SUMMARY "Unified observation, control, and dependency management. "
  DESCRIPTION ${COMMON_DESRIPTION}
)

#-------------------------------------------------------------------------------

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

