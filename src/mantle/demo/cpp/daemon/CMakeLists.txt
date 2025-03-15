## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief Build service daemon for Common Core Demo
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this executable. This also becomes the name of the project.
set(TARGET demoserver)

### Package component
if(BUILD_DEMO)
  set(INSTALL_COMPONENT "demo-server")
endif()


### Libraries we depend on, either from this build or provided by the
### system. Only direct dependencies are needed; e.g., do not list "cc_core"
### if you also list another library that depends on it.
set(LIB_DEPS
  cc_demo_native
)

if(BUILD_DDS)
  list(APPEND LIB_DEPS cc_demo_dds_service)
endif()

if(BUILD_GRPC)
  list(APPEND LIB_DEPS cc_demo_grpc_service)
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
if(INSTALL_COMPONENT)
  cc_add_executable("${TARGET}"
    LIB_DEPS ${LIB_DEPS}
    SOURCES ${SOURCES}
    DESTINATION sbin
    INSTALL_COMPONENT "${INSTALL_COMPONENT}"
  )

  if(ENABLE_SERVICES)
    cc_add_debian_service("demo"
      PROGRAM "sbin/${TARGET}"
      DESCRIPTION "DEMO service"
      INSTALL_COMPONENT "${INSTALL_COMPONENT}"
      INSTALL_GROUP "${INSTALL_GROUP}"
      ENABLE
    )
  endif()

  cpack_add_component(${INSTALL_COMPONENT}
    DEPENDS demo-settings
    GROUP ${INSTALL_GROUP}
    DISPLAY_NAME "DEMO example - C++ server"
    DESCRIPTION "DEMO application server written in C++"
  )
endif()
