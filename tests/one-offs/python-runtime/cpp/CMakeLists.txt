## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description Build main application
## @author Tor Slettnes
#===============================================================================

### Name of this executable. This also becomes the name of the project.
set(TARGET pickletest)

### Libraries we depend on, either from this build or provided by the system.
set(LIB_DEPS
  cc_core
  cc_core_io_python
)

### Source files
set(SOURCES
  main.c++
)

## Invoke common CMake rules to build executable
BuildExecutable("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
  INSTALL
)
