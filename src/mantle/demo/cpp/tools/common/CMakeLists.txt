## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build DDS Demo tool
## @author Tor Slettnes
#===============================================================================

### Name of this executable. This also becomes the name of the project.
set(TARGET cc_demo_tool_common)

### Libraries we depend on, either from this build or provided by the
### system.
set(LIB_DEPS
  cc_demo_api)

### Source files
set(SOURCES
  commands.c++
  options.c++
  )

## Invoke common CMake rules to build executable
cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
