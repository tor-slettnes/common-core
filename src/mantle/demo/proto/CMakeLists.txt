## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build protocol buffer bindings
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### This can be used as a downstream dependency.
set(TARGET cc_demo_proto)

### What type of library are we building?
set(LIB_TYPE STATIC)

### Static/shared library dependencies.
set(PROTO_DEPS cc_core_proto)

### Source files
set(SOURCES
  demo.proto
)

### Invoke common CMake rules
include(BuildProto)
