## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library.
set(TARGET cc_core_messaging_zmq_pypickle)

### Library type
set(LIB_TYPE STATIC)

### Library dependencies.
set(LIB_DEPS
  cc_core_messaging_zmq_base
  cc_core_io_python
)

### Source files in this folder
set(SOURCES
  zmq-pickle-messagebase.c++
  zmq-pickle-messagehandler.c++
  zmq-pickle-messagewriter.c++
)

### Recepies to build the library
include(BuildLibrary)

