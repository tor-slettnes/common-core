## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this executable. This also becomes the name of the project.
set(TARGET log-tool)

### Libraries we depend on, either from this build or provided by the
### system.
set(LIB_DEPS
  cc_logger_grpc_client
)

### Source files
set(SOURCES
  main.c++
  options.c++
  commands.c++
  )

## Invoke common CMake rules to build executable
# cc_add_executable("${TARGET}"
#   LIB_DEPS ${LIB_DEPS}
#   SOURCES ${SOURCES}
#   INSTALL_COMPONENT "${INSTALL_COMPONENT}"
# )

