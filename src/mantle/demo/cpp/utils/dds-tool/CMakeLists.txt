## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build DDS Demo tool
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this executable. This also becomes the name of the project.
set(TARGET demo-dds-tool)

### Libraries we depend on, either from this build or provided by the
### system.
set(LIB_DEPS
  cc_demo_dds_client
  cc_demo_native
  cc_demo_tool_common
)

### Source files
set(SOURCES
  main.c++
  )

## Invoke common CMake rules to build executable
BuildExecutable(${TARGET}
  SOURCES ${SOURCES}
  LIB_DEPS ${LIB_DEPS}
  INSTALL_COMPONENT "${INSTALL_COMPONENT}"
)
