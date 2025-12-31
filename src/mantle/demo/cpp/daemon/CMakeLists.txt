## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief Build service daemon for Common Core Demo
## @author Tor Slettnes
#===============================================================================

### Name of this executable. This also becomes the name of the project.
set(TARGET "demoserver")

### Libraries we depend on, either from this build or provided by the
### system. Only direct dependencies are needed; e.g., do not list "cc_core"
### if you also list another library that depends on it.
set(LIB_DEPS
  cc_demo_native
  cc_core_platform
)

if(BUILD_DDS)
  list(APPEND LIB_DEPS cc_demo_dds_service)
endif()

if(BUILD_GRPC)
  list(APPEND LIB_DEPS
    cc_demo_grpc_service
  )
endif()

if(BUILD_ZMQ)
  list(APPEND LIB_DEPS cc_demo_zmq_service)
endif()

### Source files
set(SOURCES
  main.c++
  options.c++
)

## Invoke common CMake rules to build executable
if(SERVER_INSTALL_COMPONENT)
  cc_add_executable("${TARGET}"
    LIB_DEPS ${LIB_DEPS}
    SOURCES ${SOURCES}
    DESTINATION sbin
    INSTALL_COMPONENT "${SERVER_INSTALL_COMPONENT}"
  )

  if(ENABLE_SERVICES)
    cc_add_debian_service("${ORGANIZATION}-demo"
      PROGRAM "sbin/${TARGET}"
      DESCRIPTION "DEMO service"
      INSTALL_COMPONENT "${SERVER_INSTALL_COMPONENT}"
      ENABLE
      RESTART
    )
  endif()
endif()
