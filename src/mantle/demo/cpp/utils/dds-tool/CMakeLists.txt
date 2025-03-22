## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build DDS Demo tool
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
  cc_core_platform
)

### Source files
set(SOURCES
  main.c++
  )

## Invoke common CMake rules to build executable
cc_add_executable(${TARGET}
  SOURCES ${SOURCES}
  LIB_DEPS ${LIB_DEPS}
  INSTALL_COMPONENT "${TOOLS_INSTALL_COMPONENT}"
)
