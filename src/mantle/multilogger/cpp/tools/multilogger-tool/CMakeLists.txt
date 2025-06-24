## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this executable. This also becomes the name of the project.
set(TARGET multilogger-tool)

### Libraries we depend on, either from this build or provided by the
### system.
set(LIB_DEPS
  cc_multilogger_grpc_client
  cc_multilogger_zmq_client
  cc_core_platform
)

### Source files
set(SOURCES
  main.c++
  implementations.c++
  options.c++
  commands.c++
  )

## Invoke common CMake rules to build executable
cc_add_executable("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
  INSTALL_COMPONENT "${TOOLS_INSTALL_COMPONENT}"
)

