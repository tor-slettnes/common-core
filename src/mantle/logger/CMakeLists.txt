## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(INSTALL_GROUP "logger")

add_subdirectory(settings)
add_subdirectory(proto)
add_subdirectory(python)
add_subdirectory(cpp)

cpack_add_component_group(${INSTALL_GROUP}
  DISPLAY_NAME "Logging service"
  DESCRIPTION "Record events such as diagnostic messages, errors, and telemetry."
)
