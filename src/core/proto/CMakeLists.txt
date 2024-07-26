## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build protocol buffer bindings
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### This can be used as a downstream dependency.
set(TARGET "cc_core_proto")

### Package component
set(INSTALL_COMPONENT "core-proto")

### Source files
set(SOURCES
  variant.proto
  status.proto
  signal.proto
  request_reply.proto
  version.proto
)

BuildProto("${TARGET}"
  LIB_TYPE STATIC
  SOURCES "${SOURCES}"
  INSTALL_COMPONENT "${INSTALL_COMPONENT}"
)

cpack_add_component(${INSTALL_COMPONENT}
  GROUP ${PKG_GROUP}
  DISPLAY_NAME "Common ProtoBuf/gRPC modules"
  DESCRIPTION "Generated Protocol Buffers/gRPC modules for common components"
)
