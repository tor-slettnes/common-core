## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build protocol buffer bindings
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### This can be used as a downstream dependency.
set(TARGET "cc_demo_proto")

### Package component
#set(INSTALL_COMPONENT "demo-proto")
set(INSTALL_COMPONENT "")

### Static/shared library dependencies.
set(PROTO_DEPS
  cc_core_proto
)

### Source files
set(SOURCES
  demo.proto
)

### Generate language bindings
BuildProto("${TARGET}"
  LIB_TYPE STATIC
  PROTO_DEPS ${PROTO_DEPS}
  SOURCES ${SOURCES}
  INSTALL_COMPONENT "${INSTALL_COMPONENT}"
)

# cpack_add_component(${INSTALL_COMPONENT}
#   DEPENDS core-proto
#   GROUP ${INSTALL_GROUP}
#   DISPLAY_NAME "DEMO Example - Generated ProtoBuf/gRPC modules"
#   DESCRIPTION "Generated runtime modules for communication with DEMO example over ProtoBuf/gRPC"
# )
