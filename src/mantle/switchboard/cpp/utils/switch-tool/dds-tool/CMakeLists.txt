## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build switch control tool
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this executable. This also becomes the name of the project.
set(TARGET switch-dds-tool)

### Libraries we depend on, either from this build or provided by the
### system.
set(LIB_DEPS
  cc_switch_tool_common
  cc_switchboard_dds_client
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
  INSTALL_COMPONENT "${INSTALL_COMPONENT}"
)
