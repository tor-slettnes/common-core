## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library.
set(TARGET cc_core_io_python_base)

### Library type
set(LIB_TYPE STATIC)

### Library dependencies.
set(LIB_DEPS
  cc_core_messaging_common
  cc_core_common
)

### Third-party package dependencies
set(PKG_DEPS
  python3
)

### Source files in this folder
set(SOURCES
  python-simpleobject.c++
  python-containerobject.c++
  python-builtin.c++
)

### Recepies to build the library
include(BuildLibrary)

