## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(INSTALL_GROUP "logger")

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


if(INSTALL_GROUP)
  cpack_add_component_group(${INSTALL_GROUP}
    DISPLAY_NAME "Logging service"
    DESCRIPTION "Record events such as diagnostic messages, errors, and telemetry."
  )
endif()

