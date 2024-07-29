## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build protocol buffer bindings
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### This can be used as a downstream dependency.
set(TARGET "cc_platform_proto")

### Package component
set(INSTALL_COMPONENT "platform-proto")

### Static/shared library dependencies.
set(LIB_DEPS cc_core_proto)

### Source files
set(SOURCES
  sysconfig.proto
  netconfig.proto
  vfs.proto
)

### Generate language bindings
BuildProto("${TARGET}"
  LIB_TYPE STATIC
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
  INSTALL_COMPONENT "${INSTALL_COMPONENT}"
)

cpack_add_component(${INSTALL_COMPONENT}
  DEPENDS core-proto
  GROUP ${PKG_GROUP}
  DISPLAY_NAME "Common Core Platform services - ProtoBuf/gRPC modules"
  DESCRIPTION "Generated runtime modules for communication with "
  "Common Core platform services over ProtoBuf/gRPC."
)
