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
  cc/demo/demo.proto
)

### Generate language bindings
cc_add_proto("${TARGET}"
  PROTOS ${SOURCES}
  BASE_DIR "${CC_PROTO_IMPORT_DIR}"
  PROTO_DEPS ${PROTO_DEPS}
  PYTHON_INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}"
)
