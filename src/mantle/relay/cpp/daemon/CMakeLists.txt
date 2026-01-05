## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief Build service daemon for MultiLogger Server
## @author Tor Slettnes
#===============================================================================

### Name of this executable. This also becomes the name of the project.
set(TARGET ${EXEC_NAME_PREFIX}message-relay)

### Libraries we depend on, either from this build or provided by the
### sysconfig. Only direct dependencies are needed; e.g., do not list "cc_core"
### if you also list another library that depends on it.
set(LIB_DEPS
  cc_relay_grpc_service
  cc_relay_zmq_run
  cc_core_platform
)

### Source files
set(SOURCES
  options.c++
  main.c++
)

## Invoke common CMake rules to build executable
cc_add_executable("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
  DESTINATION sbin
  INSTALL_COMPONENT "${SERVER_INSTALL_COMPONENT}"
)

if(ENABLE_SERVICES)
  cc_add_debian_service("${TARGET}"
    PROGRAM "sbin/${TARGET}"
    DESCRIPTION "${COMMON_SUMMARY}"
    INSTALL_COMPONENT "${SERVER_INSTALL_COMPONENT}"
    ENABLE
  )
endif()
