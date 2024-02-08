## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library.
set(TARGET cc_shared_ipc_python)

### Library type
set(LIB_TYPE STATIC)

### Library dependencies.
set(LIB_DEPS
  cc_shared_ipc_common
  cc_shared_platform
)

### Third-party package dependencies
set(PKG_DEPS
  python3-embed
)

### Source files in this folder
set(SOURCES
  python-simpleobject.c++
  python-containerobject.c++
  python-exception.c++
  python-runtime.c++
  python-pickle.c++
)

### Recepies to build the library
include(BuildLibrary)

