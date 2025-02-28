## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(INSTALL_GROUP "multilogger")

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
  cc_cpack_add_group(${INSTALL_GROUP}
    DISPLAY_NAME "MultiLogger service"
    DESCRIPTION "Record events such as diagnostic messages, errors, and telemetry."
    GROUP_DEPS "common"
  )
endif()

