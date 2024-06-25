## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build protocol buffer bindings
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### This can be used as a downstream dependency.
set(TARGET "cc_demo_proto")

### Package component
set(PKG_COMPONENT "demo-proto")

### Static/shared library dependencies.
set(LIB_DEPS cc_core_proto)

### Source files
set(SOURCES
  demo.proto
)

### Generate language bindings
BuildProto("${TARGET}"
  LIB_TYPE STATIC
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
  COMPONENT "${PKG_COMPONENT}"
)

cpack_add_component(${PKG_COMPONENT}
  DEPENDS core-proto
  GROUP ${PKG_GROUP}
  DISPLAY_NAME "DEMO Example - Generated ProtoBuf/gRPC modules"
  DESCRIPTION "Generated runtime modules for communication with DEMO example over ProtoBuf/gRPC"
)
