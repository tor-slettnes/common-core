## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

if(BUILD_MULTILOGGER)
  set(INSTALL_GROUP "multilogger")

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
  SUMMARY "Common Core - MultiLogger service settings"
  DESCRIPTION "Settings for Common Core MultiLogger service."
)

cc_cpack_add_debian_component("${PYTHON_INSTALL_COMPONENT}"
  GROUP "${INSTALL_GROUP}"
  DEPENDS "${SETTINGS_INSTALL_COMPONENT}" "core-python"
  SUMMARY "Common Core - Python client for MultiLogger service"
  DESCRIPTION "Python client module for Common Core MultiLogger service."
)

cc_cpack_add_debian_component("${TOOLS_INSTALL_COMPONENT}"
  GROUP "${INSTALL_GROUP}"
  DEPENDS "${SETTINGS_INSTALL_COMPONENT}"
  SUMMARY "Common Core - MultiLogger service command line tools"
  DESCRIPTION "Multilogger command-line utlities written in C++"
)

cc_cpack_add_debian_component("${SERVER_INSTALL_COMPONENT}"
  GROUP "${INSTALL_GROUP}"
  DEPENDS "${SETTINGS_INSTALL_COMPONENT}"
  SUMMARY "Common Core - MultiLogger server"
  DESCRIPTION "Record events such as diagnostic messages, errors, and telemetry."
)

cc_cpack_add_group("${INSTALL_GROUP}"
  GROUP_DEPS "core"
  SUMMARY "Common Core - MultiLogger"
  DESCRIPTION "Record events such as diagnostic messages, errors, and telemetry."
)
