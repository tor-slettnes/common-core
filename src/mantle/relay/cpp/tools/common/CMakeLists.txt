## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules Relay tool - common components
## @author Tor Slettnes
#===============================================================================

### Name of this executable. This also becomes the name of the project.
set(TARGET cc_relay_tools_common)

### Libraries we depend on, either from this build or provided by the
### system.
set(LIB_DEPS
  cc_relay_common
)

### Source files
set(SOURCES
  common-options.c++
  )

## Invoke common CMake rules to build executable
cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
