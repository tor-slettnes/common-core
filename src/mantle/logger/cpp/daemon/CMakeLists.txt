## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description Build service daemon for Logger Server
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this executable. This also becomes the name of the project.
set(TARGET logserver)

set(INSTALL_COMPONENT "log-server")

### Libraries we depend on, either from this build or provided by the
### sysconfig. Only direct dependencies are needed; e.g., do not list "cc_core"
### if you also list another library that depends on it.
set(LIB_DEPS
  cc_core_platform
  cc_logger_native
  cc_logger_grpc_service
)

### Source files
set(SOURCES
  grpc-service.c++
  options.c++
  main.c++
)

## Invoke common CMake rules to build executable
cc_add_executable("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
  DESTINATION sbin
  INSTALL_COMPONENT "${INSTALL_COMPONENT}"
)

if(ENABLE_SERVICES)
  cc_add_debian_service("logserver"
    PROGRAM "sbin/${TARGET}"
    DESCRIPTION "Record events such as diagnostic messages, errors, and telemetry."
    INSTALL_COMPONENT "${INSTALL_COMPONENT}"
    INSTALL_GROUP "${INSTALL_GROUP}"
    ENABLE
  )
endif()

cpack_add_component(${INSTALL_COMPONENT}
  DEPENDS "logger-settings"
  GROUP ${INSTALL_GROUP}
  DISPLAY_NAME "Log server"
  DESCRIPTION "Log server: Record events such as diagnostic messages, errors, and telemetry"
)
