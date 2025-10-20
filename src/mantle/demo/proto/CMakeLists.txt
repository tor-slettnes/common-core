## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build protocol buffer bindings
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### This can be used as a downstream dependency.
set(TARGET "cc_demo_proto")

### Static/shared library dependencies.
set(PROTO_DEPS
  cc_core_proto
)

### Source files
set(SOURCES
  demo.proto
)

### Generate language bindings
cc_add_proto("${TARGET}"
  PROTO_DEPS ${PROTO_DEPS}
  PROTOS ${SOURCES}
  PYTHON_INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}"
)
