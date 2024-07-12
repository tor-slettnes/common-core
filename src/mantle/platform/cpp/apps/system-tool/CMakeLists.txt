## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build System gRPC client tool
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this executable. This also becomes the name of the project.
set(TARGET system-tool)

### Libraries we depend on, either from this build or provided by the
### system.
set(LIB_DEPS
  cc_core_platform
  cc_platform_system_native
  cc_platform_system_grpc_client
)

### Source files
set(SOURCES
  main.c++
  options.c++
  commands.c++
  )

## Invoke common CMake rules to build executable
BuildExecutable("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
  COMPONENT ${PKG_COMPONENT}
)
