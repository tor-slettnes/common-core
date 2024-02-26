## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library.
set(TARGET cc_core_io_python_embedded)

### Library type
set(LIB_TYPE STATIC)

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
include(BuildLibrary)

