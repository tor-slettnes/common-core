## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

if(BUILD_SHARED)
  set(INSTALL_GROUP "core")

  set(SETTINGS_INSTALL_COMPONENT "${INSTALL_GROUP}-settings")

  if(BUILD_PYTHON)
    set(PYTHON_INSTALL_COMPONENT "${INSTALL_GROUP}-python")
    if(INSTALL_PYTHON_REQUIREMENTS)
      set(PYTHON_REQUIREMENTS_INSTALL_COMPONENT "${INSTALL_GROUP}-python-requirements")
    endif()
  endif()
endif()

### Install "settings/" folder as data
add_subdirectory(settings)

if(BUILD_DDS)
  add_subdirectory(idl)
endif()

if(BUILD_PROTOBUF)
  add_subdirectory(proto)
endif()

if(BUILD_CPP)
  add_subdirectory(cpp)
endif()

if(BUILD_PYTHON)
  add_subdirectory(python)
endif()


cc_cpack_add_debian_component("${SETTINGS_INSTALL_COMPONENT}"
  GROUP "${INSTALL_GROUP}"
  SUMMARY "Common Core - Common settings"
  DESCRIPTION "Shared settings files for Common Core application framework."
)

cc_cpack_add_debian_component("${PYTHON_REQUIREMENTS_INSTALL_COMPONENT}"
  GROUP "${INSTALL_GROUP}"
  SUMMARY "Common Core - Required 3rd party Python distributions"
  DESCRIPTION "Required 3rd party Python distributions for shared Python modules."
)

cc_cpack_add_debian_component("${PYTHON_INSTALL_COMPONENT}"
  GROUP "${INSTALL_GROUP}"
  SUMMARY "Common Core - Shared Python modules"
  DESCRIPTION "Common Core application framework for Python."
  DEPENDS ${SETTINGS_INSTALL_COMPONENT} ${PYTHON_REQUIREMENTS_INSTALL_COMPONENT}
)

cc_cpack_add_group("${INSTALL_GROUP}"
  SUMMARY "Common Core - Shared components"
)
