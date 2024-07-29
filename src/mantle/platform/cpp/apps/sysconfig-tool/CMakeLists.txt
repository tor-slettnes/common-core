## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build SysConfig gRPC client tool
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET sysconfig-tool)

set(LIB_DEPS
  cc_core_platform
  cc_platform_sysconfig_native
  cc_platform_sysconfig_grpc_client
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
