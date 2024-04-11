## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this executable. This also becomes the name of the project.
set(TARGET dt-test)

### Libraries we depend on, either from this build or provided by the
### system.
set(LIB_DEPS
  cc_core_platform
)

### Source files
set(SOURCES
  main.c++
  )

## Invoke common CMake rules to build executable
build_executable("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
  INSTALL
)

