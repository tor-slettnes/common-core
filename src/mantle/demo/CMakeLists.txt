## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Common Core Demo
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

if(BUILD_DEMO)
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
endif()

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

if (BUILD_CPP)
  add_subdirectory(cpp)
endif()

if (BUILD_PYTHON)
  add_subdirectory(python)
endif()

cc_install_doc(
  FILES ${CMAKE_SOURCE_DIR}/LICENSE.txt
  RENAME copyright
  COMPONENT ${COMPLETE_INSTALL_COMPONENT}
  GROUP ${INSTALL_GROUP}
)

cc_cpack_add_debian_component("${SETTINGS_INSTALL_COMPONENT}"
  GROUP "${INSTALL_GROUP}"
  DEPENDS "core-settings"
  SUMMARY "Common Core - DEMO service settings"
  DESCRIPTION "Settings for Common Core DEMO service."
)

cc_cpack_add_debian_component("${PYTHON_INSTALL_COMPONENT}"
  GROUP "${INSTALL_GROUP}"
  DEPENDS "${SETTINGS_INSTALL_COMPONENT}" "core-python"
  SUMMARY "Common Core DEMO - launcher scripts"
  DESCRIPTION "Shell script to launch interactive Python shell for the Common Core DEMO service"
)

cc_cpack_add_debian_component("${TOOLS_INSTALL_COMPONENT}"
  GROUP "${INSTALL_GROUP}"
  DEPENDS "${SETTINGS_INSTALL_COMPONENT}"
  SUMMARY "Common Core DEMO - command-line utilities"
  DESCRIPTION "DEMO command-line utilities written in C++"
)

cc_cpack_add_debian_component("${SERVER_INSTALL_COMPONENT}"
  GROUP "${INSTALL_GROUP}"
  DEPENDS "${SETTINGS_INSTALL_COMPONENT}"
  SUMMARY "Common Core DEMO - server"
  DESCRIPTION "DEMO server written in C++"
)

cc_cpack_add_debian_component("${COMPLETE_INSTALL_COMPONENT}"
  GROUP "${INSTALL_GROUP}"
  DEPENDS ${SETTINGS_INSTALL_COMPONENT} ${PYTHON_INSTALL_COMPONENT} ${TOOLS_INSTALL_COMPONENT} ${SERVER_INSTALL_COMPONENT}
  SUMMARY "Common Core DEMO - complete"
  DESCRIPTION "This is a meta-package whose sole purpose is to depend on all DEMO packages"
)

cc_cpack_add_group("${INSTALL_GROUP}"
  GROUP_DEPS "core"
  SUMMARY "DEMO Example"
  DESCRIPTION "DEMO application example"
)
