## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description Build service daemon for Platform Server
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this executable. This also becomes the name of the project.
set(TARGET platformserver)

### Libraries we depend on, either from this build or provided by the
### system. Only direct dependencies are needed; e.g., do not list "cc_core"
### if you also list another library that depends on it.
set(LIB_DEPS
  cc_core_platform
  cc_platform_system_native
  cc_platform_network_dbus
  cc_platform_system_grpc_service
  cc_platform_network_grpc_service
)

### Source files
set(SOURCES
  platform-grpc-server.c++
  platform-glib-mainloop.c++
  options.c++
  main.c++
)

## Invoke common CMake rules to build executable
BuildExecutable("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
  DESTINATION sbin
  COMPONENT ${PKG_COMPONENT}
)

if(ENABLE_SERVICES)
  InstallDebianService("platformserver"
    PROGRAM "sbin/${TARGET}"
    DESCRIPTION "Platform server: System configuration, Networking, File access, Upgrade"
    COMPONENT "${PKG_COMPONENT}"
  )
endif()

cpack_add_component(${PKG_COMPONENT}
  DEPENDS core-common
  GROUP ${PKG_GROUP}
  DISPLAY_NAME "Platform server"
  DESCRIPTION "Platform services: System configuration, networking, filesystem, upgrade, logging"
)
