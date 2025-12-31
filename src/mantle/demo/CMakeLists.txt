## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Common Core Demo
## @author Tor Slettnes
#===============================================================================

if(BUILD_DEMO)
  set(COMMON_DESCRIPTION
    "This demo application is provided to illustrate the morphology of a typical"
    "microservice applications based on the Common Core application framework."
    "It may be used as a template for creating new applications, or as a reference"
    "implementation of current best practices."
  )

  set(INSTALL_GROUP "demo")

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
  SUMMARY "Common Core DEMO - service settings"
  DESCRIPTION ${COMMON_DESCRIPTION}
  "."
  "This package contains settings files."
)

cc_cpack_add_debian_component("${PYTHON_INSTALL_COMPONENT}"
  GROUP "${INSTALL_GROUP}"
  DEPENDS "${SETTINGS_INSTALL_COMPONENT}" "core-python"
  LICENSE ${LICENSE_FILE}
  SUMMARY "Common Core DEMO - Python client package"
  DESCRIPTION ${COMMON_DESCRIPTION}
  "."
  "This package contains Python client modules."
)

cc_cpack_add_debian_component("${TOOLS_INSTALL_COMPONENT}"
  GROUP "${INSTALL_GROUP}"
  DEPENDS "${SETTINGS_INSTALL_COMPONENT}"
  LICENSE ${LICENSE_FILE}
  SUMMARY "Common Core DEMO - command-line utilities"
  DESCRIPTION ${COMMON_DESCRIPTION}
  "."
  "This package contains command-line client tools written in C++."
)

cc_cpack_add_debian_component("${SERVER_INSTALL_COMPONENT}"
  GROUP "${INSTALL_GROUP}"
  DEPENDS "${SETTINGS_INSTALL_COMPONENT}"
  RECOMMENDS "${TOOLS_INSTALL_COMPONENT}"
  DEB_RECOMMENDS ${PACKAGE_NAME_PREFIX}-multilogger-server
  LICENSE ${LICENSE_FILE}
  SUMMARY "Common Core DEMO - server"
  DESCRIPTION ${COMMON_DESCRIPTION}
  "."
  "This package contains the server daemon, written in C++."
)

cc_cpack_add_debian_component("${COMPLETE_INSTALL_COMPONENT}"
  GROUP "${INSTALL_GROUP}"
  DEPENDS ${COMPLETE_INSTALL_DEPENDENCIES}
  LICENSE ${LICENSE_FILE}
  SUMMARY "Common Core DEMO - complete"
  DESCRIPTION ${COMMON_DESCRIPTION}
  "."
  "This meta-package depends on and will thus install all DEMO packages."
  "You may choose to select specific component packages instead."
)

cc_cpack_add_group("${INSTALL_GROUP}"
  GROUP_DEPENDS "core"
  SUMMARY "DEMO Example"
  DESCRIPTION ${COMMON_DESCRIPTION}
)


#-------------------------------------------------------------------------------

add_subdirectory(settings)
add_subdirectory(scripts)

### Each of the following subdirectories contain targets that may be needed by
### downstream targets, even if we don't build the DEMO service itself.

if (BUILD_PROTOBUF)
  add_subdirectory(proto)
endif()

if (BUILD_DDS)
  add_subdirectory(idl)
endif()

if (BUILD_PYTHON)
  add_subdirectory(python)
endif()

if (BUILD_CPP)
  add_subdirectory(cpp)
endif()
