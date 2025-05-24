## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build switchboard service daemon
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this executable. This also becomes the name of the project.
set(TARGET switchboard)

set(LIB_DEPS
  cc_switchboard_native
  cc_core_platform
)

set(SOURCES
  main.c++
  options.c++
  )

if(BUILD_DDS)
  list(APPEND LIB_DEPS
    cc_switchboard_dds_publisher
    cc_switchboard_dds_service
  )

  list(APPEND SOURCES
    switchboard-dds-run.c++
  )
endif()

if(BUILD_GRPC)
  list(APPEND LIB_DEPS
    cc_switchboard_grpc_service
    cc_multilogger_grpc_client
  )

  list(APPEND SOURCES
    switchboard-grpc-run.c++
  )
endif()

## Invoke common CMake rules to build executable
if(SERVER_INSTALL_COMPONENT)
  cc_add_executable("${TARGET}"
    LIB_DEPS ${LIB_DEPS}
    SOURCES ${SOURCES}
    DESTINATION sbin
    INSTALL_COMPONENT "${SERVER_INSTALL_COMPONENT}"
  )

  if(ENABLE_SERVICES)
    cc_add_debian_service("${TARGET}"
      PROGRAM "sbin/${TARGET}"
      DESCRIPTION "Switchboard service"
      INSTALL_COMPONENT "${SERVER_INSTALL_COMPONENT}"
      ENABLE
    )
  endif()
endif()
