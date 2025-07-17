## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

if(BUILD_MULTILOGGER)
  set(COMMON_DESCRIPTION
    "MultiLogger provides consolidated recording of events such as diagnostic"
    "messages, errors, and telemetry for Common Core applications."
    "."
    "Based on 'contract id' identifying specific log types, events are captured"
    "in zero or more 'sinks'. This may include:"
    "  - console output (stdout/stderr)"
    "  - UNIX syslog or Windows Event log (unstructured text only)"
    "  - various file formats (text logs, csv, JSON...)"
    "  - database (currently SQLite3)"
  )

  set(INSTALL_GROUP "multilogger")

  set(SETTINGS_INSTALL_COMPONENT "${INSTALL_GROUP}-settings")

  if(BUILD_PYTHON)
    set(PYTHON_INSTALL_COMPONENT "${INSTALL_GROUP}-python")
  endif()

  if(BUILD_CPP)
    set(TOOLS_INSTALL_COMPONENT "${INSTALL_GROUP}-tools")
    set(SERVER_INSTALL_COMPONENT "${INSTALL_GROUP}-server")
  endif()

  set(COMPLETE_INSTALL_COMPONENT "${INSTALL_GROUP}-complete")
  set(COMPLETE_INSTALL_DEPENDENCIES
    ${SETTINGS_INSTALL_COMPONENT}
    ${PYTHON_INSTALL_COMPONENT}
    ${TOOLS_INSTALL_COMPONENT}
    ${SERVER_INSTALL_COMPONENT}
  )
endif()

#-------------------------------------------------------------------------------

cc_cpack_add_debian_component("${SETTINGS_INSTALL_COMPONENT}"
  GROUP "${INSTALL_GROUP}"
  DEPENDS "core-settings"
  LICENSE ${LICENSE_FILE}
  SUMMARY "Common Core MultiLogger - settings"
  DESCRIPTION ${COMMON_DESCRIPTION}
  "."
  "This package contains settings files."
)

cc_cpack_add_debian_component("${PYTHON_INSTALL_COMPONENT}"
  GROUP "${INSTALL_GROUP}"
  DEPENDS "${SETTINGS_INSTALL_COMPONENT}" "core-python"
  LICENSE ${LICENSE_FILE}
  SUMMARY "Common Core MultiLogger - Python client package"
  DESCRIPTION ${COMMON_DESCRIPTION}
  "."
  "This package contains Python client modules."
)

cc_cpack_add_debian_component("${TOOLS_INSTALL_COMPONENT}"
  GROUP "${INSTALL_GROUP}"
  DEPENDS "${SETTINGS_INSTALL_COMPONENT}"
  LICENSE ${LICENSE_FILE}
  SUMMARY "Common Core MultiLogger - command line tools"
  DESCRIPTION ${COMMON_DESCRIPTION}
  "."
  "This package contains command-line client tools written in C++."
)

cc_cpack_add_debian_component("${SERVER_INSTALL_COMPONENT}"
  GROUP "${INSTALL_GROUP}"
  DEPENDS "${SETTINGS_INSTALL_COMPONENT}"
  SUGGESTS "${TOOLS_INSTALL_COMPONENT}"
  LICENSE ${LICENSE_FILE}
  SUMMARY "Common Core MultiLogger - server"
  DESCRIPTION ${COMMON_DESCRIPTION}
  "."
  "This package contains the server daemon, written in C++."
)

cc_cpack_add_debian_component("${COMPLETE_INSTALL_COMPONENT}"
  GROUP "${INSTALL_GROUP}"
  DEPENDS ${COMPLETE_INSTALL_DEPENDENCIES}
  LICENSE ${LICENSE_FILE}
  SUMMARY "Common Core MultiLogger - complete"
  DESCRIPTION ${COMMON_DESCRIPTION}
  "."
  "This meta-package depends on and will thus install all MultiLogger packages."
  "You may choose to select specific component packages instead."
)

cc_cpack_add_group("${INSTALL_GROUP}"
  GROUP_DEPENDS "core"
  SUMMARY "Capture logs from Common Core microservice applications"
  DESCRIPTION ${COMMON_DESCRIPTION}
)

#-------------------------------------------------------------------------------

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
