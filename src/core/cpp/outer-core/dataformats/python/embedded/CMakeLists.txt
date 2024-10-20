## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library.
set(TARGET cc_core_io_python_embedded)

### Library dependencies.
set(LIB_DEPS
  cc_core_io_python_base
)

### Source files in this folder
set(SOURCES
  python-exception.c++
  python-runtime.c++
  python-pickle.c++
)

### Recepies to build the library
cc_add_library("${TARGET}"
  LIB_TYPE STATIC
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
