## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build NetConfig gRPC client tool
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET netconfig-tool)

set(LIB_DEPS
  cc_core_platform
  cc_platform_netconfig_dbus
  cc_platform_netconfig_grpc_client
)

set(SOURCES
  main.c++
  options.c++
  commands.c++
  )

BuildExecutable("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
  INSTALL_COMPONENT "${INSTALL_COMPONENT}"
)
