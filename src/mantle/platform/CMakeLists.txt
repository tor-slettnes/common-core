## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Common Core components
## @author Tor Slettnes
#===============================================================================

if(BUILD_PLATFORM)
  set(COMMON_DESRIPTION
    "Microservice application for generic system configuration, "
    "network configuration, remote file access, and software upgrade."
  )

  set(INSTALL_GROUP "platform")

  set(SETTINGS_INSTALL_COMPONENT "${INSTALL_GROUP}-settings")

  if(BUILD_PYTHON)
    set(PYTHON_INSTALL_COMPONENT "${INSTALL_GROUP}-python")

    if (INSTALL_PYTHON_MODULES)
      set(PYTHON_MODULES_INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}")
    endif()

    if (INSTALL_PYTHON_WHEELS)
      set(PYTHON_WHEELS_INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}")
    endif()

    if (INSTALL_PYTHON_EXECUTABLES)
      set(PYTHON_BINARY_INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}")
    endif()


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
  SUMMARY "Common Core Platform Service - settings"
  DESCRIPTION ${COMMON_DESCRIPTION}
  "."
  "This package contains settings files."
)

cc_cpack_add_debian_component("${PYTHON_INSTALL_COMPONENT}"
  GROUP "${INSTALL_GROUP}"
  DEPENDS "${SETTINGS_INSTALL_COMPONENT}" "core-python"
  LICENSE ${LICENSE_FILE}
  SUMMARY "Common Core Platform Services - Python clients"
  DESCRIPTION ${COMMON_DESCRIPTION}
  "."
  "This package contains Python client modules, as well as a"
  "'platform-shell' utility to launch an interactive Python prompt"
  "with these client modules preloaded."
)

cc_cpack_add_debian_component("${TOOLS_INSTALL_COMPONENT}"
  GROUP "${INSTALL_GROUP}"
  DEPENDS "${SETTINGS_INSTALL_COMPONENT}"
  LICENSE ${LICENSE_FILE}
  SUMMARY "Common Core Platform Services - command line tools"
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
  SUMMARY "Common Core Platform Services - server"
  DESCRIPTION ${COMMON_DESCRIPTION}
  "."
  "This package contains the `platformserver` daemon, written in C++."
)

cc_cpack_add_debian_component("${COMPLETE_INSTALL_COMPONENT}"
  GROUP "${INSTALL_GROUP}"
  DEPENDS ${COMPLETE_INSTALL_DEPENDENCIES}
  LICENSE ${LICENSE_FILE}
  SUMMARY "Common Core Platform Services - complete"
  DESCRIPTION ${COMMON_DESCRIPTION}
  "."
  "This meta-package depends on and will thus install all Platform packages."
  "You may choose to select specific component packages instead."
)

cc_cpack_add_group("${INSTALL_GROUP}"
  GROUP_DEPENDS "core"
  SUMMARY "Platform services"
  DESCRIPTION ${COMMON_DESRIPTION}
)

#-------------------------------------------------------------------------------

add_subdirectory(settings)
add_subdirectory(scripts)

if (BUILD_PROTOBUF)
  add_subdirectory(proto)
endif()

if (BUILD_CPP)
  add_subdirectory(cpp)
endif()

if (BUILD_PYTHON)
  add_subdirectory(python)
endif()
