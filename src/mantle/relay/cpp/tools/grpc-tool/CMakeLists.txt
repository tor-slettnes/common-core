## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build gRPC Relay tool
## @author Tor Slettnes
#===============================================================================

### Name of this executable. This also becomes the name of the project.
set(TARGET ${EXEC_NAME_PREFIX}relay-grpc-tool)

### Libraries we depend on, either from this build or provided by the
### system.
set(LIB_DEPS
  cc_relay_grpc_client
  cc_relay_tools_common
  cc_core_platform
)

### Source files
set(SOURCES
  main.c++
  options.c++
  )

## Invoke common CMake rules to build executable
cc_add_executable(${TARGET}
  SOURCES ${SOURCES}
  LIB_DEPS ${LIB_DEPS}
  INSTALL_COMPONENT "${TOOLS_INSTALL_COMPONENT}"
)
