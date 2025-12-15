## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief Build service daemon for Platform Server
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this executable. This also becomes the name of the project.
set(TARGET "cc-platform-server")

### Libraries we depend on, either from this build or provided by the
### sysconfig. Only direct dependencies are needed; e.g., do not list "cc_core"
### if you also list another library that depends on it.
set(LIB_DEPS
  cc_core_platform
  cc_platform_sysconfig_native
  cc_platform_netconfig_dbus
  cc_platform_vfs_local
  cc_platform_upgrade_native
)

if(BUILD_GRPC)
  list(APPEND LIB_DEPS
    cc_platform_sysconfig_grpc_service
    cc_platform_netconfig_grpc_service
    cc_platform_vfs_grpc_service
    cc_platform_upgrade_grpc_service
    cc_multilogger_grpc_client
  )
endif()


### Source files
set(SOURCES
  platform-grpc-server.c++
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
    DESCRIPTION "Platform server: System configuration, Networking, File access, Upgrade"
    INSTALL_COMPONENT "${SERVER_INSTALL_COMPONENT}"
    ENABLE
  )
endif()
