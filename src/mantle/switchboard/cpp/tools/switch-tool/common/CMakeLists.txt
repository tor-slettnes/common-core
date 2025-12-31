## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build switch control tool
## @author Tor Slettnes
#===============================================================================

### Name of this executable. This also becomes the name of the project.
set(TARGET cc_switch_tool_common)

### Libraries we depend on, either from this build or provided by the
### system.
set(LIB_DEPS
  cc_switchboard_base
)

### Source files
set(SOURCES
  options.c++
  commands.c++
  )


cc_add_library(${TARGET}
  SOURCES ${SOURCES}
  LIB_DEPS ${LIB_DEPS}
)
