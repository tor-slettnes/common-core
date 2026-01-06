## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build SysConfig gRPC client tool
## @author Tor Slettnes
#===============================================================================

set(TARGET ${EXEC_NAME_PREFIX}sysconfig-tool)

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

cc_add_executable("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
  INSTALL_COMPONENT "${TOOLS_INSTALL_COMPONENT}"
)
