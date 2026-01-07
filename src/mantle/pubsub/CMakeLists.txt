## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules
## @author Tor Slettnes
#===============================================================================

if(BUILD_RELAY)
  set(COMMON_SUMMARY
    "Pub/Sub broker supporting multiple payload formats and transports")

  set(COMMON_DESCRIPTION
    "Relay is an intermediary message broker between producers and consumers of"
    "asynchronous events and data.  It serves two primary purposes:"
    "."
    "  - To provide a well-known static point in the network to which message"
    "    producers/publishers and consumers/subscribers can both connect"
    "    without being coupled tightly to each other, and"
    "."
    "  - To relay messages across format and protocol boundaries. Currently"
    "    these are: ProtoBuf over gRPC, JSON over ZMQ, JSON over WebSockets."
  )

  set(INSTALL_GROUP "pubsub")

  set(SETTINGS_INSTALL_COMPONENT "${INSTALL_GROUP}-settings")

  if(BUILD_PYTHON)
    set(PYTHON_INSTALL_COMPONENT "${INSTALL_GROUP}-python")
  endif()

  if(BUILD_CPP)
    set(TOOLS_INSTALL_COMPONENT "${INSTALL_GROUP}-tools")
    set(SERVER_INSTALL_COMPONENT "${INSTALL_GROUP}-relay")
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
  SUMMARY "${COMMON_SUMMARY} - settings"
  DESCRIPTION ${COMMON_DESCRIPTION}
  "."
  "This package contains settings files."
)

cc_cpack_add_debian_component("${PYTHON_INSTALL_COMPONENT}"
  GROUP "${INSTALL_GROUP}"
  DEPENDS "${SETTINGS_INSTALL_COMPONENT}" "core-python"
  LICENSE ${LICENSE_FILE}
  SUMMARY "${COMMON_SUMMARY} - Python client package"
  DESCRIPTION ${COMMON_DESCRIPTION}
  "."
  "This package contains Python client modules."
)

cc_cpack_add_debian_component("${TOOLS_INSTALL_COMPONENT}"
  GROUP "${INSTALL_GROUP}"
  DEPENDS "${SETTINGS_INSTALL_COMPONENT}"
  LICENSE ${LICENSE_FILE}
  SUMMARY "${COMMON_SUMMARY} - command line tools"
  DESCRIPTION ${COMMON_DESCRIPTION}
  "."
  "This package contains command-line client tools written in C++."
)

cc_cpack_add_debian_component("${SERVER_INSTALL_COMPONENT}"
  GROUP "${INSTALL_GROUP}"
  DEPENDS "${SETTINGS_INSTALL_COMPONENT}"
  SUGGESTS "${TOOLS_INSTALL_COMPONENT}"
  LICENSE ${LICENSE_FILE}
  SUMMARY "${COMMON_SUMMARY} - server"
  DESCRIPTION ${COMMON_DESCRIPTION}
  "."
  "This package contains the server daemon, written in C++."
)

cc_cpack_add_debian_component("${COMPLETE_INSTALL_COMPONENT}"
  GROUP "${INSTALL_GROUP}"
  DEPENDS ${COMPLETE_INSTALL_DEPENDENCIES}
  LICENSE ${LICENSE_FILE}
  SUMMARY "${COMMON_SUMMARY} - complete"
  DESCRIPTION ${COMMON_DESCRIPTION}
  "."
  "This meta-package depends on and will thus install all Relay packages."
  "You may choose to select specific component packages instead."
)

cc_cpack_add_group("${INSTALL_GROUP}"
  GROUP_DEPENDS "core"
  SUMMARY "Pub/Sub broker supporting multiple payload formats and transports"
  DESCRIPTION ${COMMON_DESCRIPTION}
)

#-------------------------------------------------------------------------------

add_subdirectory(settings)

if(BUILD_PROTOBUF)
  add_subdirectory(proto)
endif()

if(BUILD_CPP)
  add_subdirectory(cpp)
endif()
