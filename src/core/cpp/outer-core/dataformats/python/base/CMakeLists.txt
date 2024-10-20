## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library.
set(TARGET cc_core_io_python_base)

### We need Python 3.x for this
find_package(Python3
  REQUIRED COMPONENTS Development)

### Library dependencies.
set(LIB_DEPS
  cc_core_messaging_common
  cc_core_common
  Python3::Python
)

### Source files in this folder
set(SOURCES
  python-simpleobject.c++
  python-containerobject.c++
  python-builtin.c++
)

### Recepies to build the library
cc_add_library("${TARGET}"
  LIB_TYPE STATIC
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
