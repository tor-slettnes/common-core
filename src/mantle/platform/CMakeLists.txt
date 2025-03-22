## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

if(BUILD_PLATFORM)
  set(INSTALL_GROUP "platform")

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


cc_cpack_add_debian_component("${SETTINGS_INSTALL_COMPONENT}"
  GROUP "${INSTALL_GROUP}"
  DEPENDS "core-settings"
  SUMMARY "Common Core - Platform service settings"
  DESCRIPTION "Settings for Common Core Platform service."
)

cc_cpack_add_debian_component("${PYTHON_INSTALL_COMPONENT}"
  GROUP "${INSTALL_GROUP}"
  DEPENDS "${SETTINGS_INSTALL_COMPONENT}" "core-python"
  SUMMARY "Common Core - Python clients for platform services"
  DESCRIPTION "Python client moduules for platform services: SysConfig, NetConfig, VFS"
)

cc_cpack_add_debian_component("${TOOLS_INSTALL_COMPONENT}"
  GROUP "${INSTALL_GROUP}"
  DEPENDS "${SETTINGS_INSTALL_COMPONENT}"
  SUMMARY "Common Core - Platform service command line tools"
  DESCRIPTION "Command line tools to communiate with the Platform service via gRPC."
)

cc_cpack_add_debian_component("${SERVER_INSTALL_COMPONENT}"
  GROUP "${INSTALL_GROUP}"
  DEPENDS "${SETTINGS_INSTALL_COMPONENT}"
  SUMMARY "Common Core - Platform server"
  DESCRIPTION "Microservice application for generic system configuration, "
  "network configuration, remote file access, and software upgrade."
)

cc_cpack_add_group("${INSTALL_GROUP}"
  GROUP_DEPS "core"
  SUMMARY "Platform services"
  DESCRIPTION "Microservice application for generic system configuration, "
  "network configuration, remote file access, and software upgrade."
)
