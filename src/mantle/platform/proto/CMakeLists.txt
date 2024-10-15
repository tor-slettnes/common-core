## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build protocol buffer bindings
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### This can be used as a downstream dependency.
set(TARGET "cc_platform_proto")

### Package component
if(INCLUDE_PLATFORM)
  set(INSTALL_COMPONENT "platform-proto")
endif()

### Static/shared library dependencies.
set(PROTO_DEPS cc_core_proto)

### Source files
set(SOURCES
  sysconfig.proto
  netconfig.proto
  vfs.proto
  upgrade.proto
)

### Generate language bindings
BuildProto("${TARGET}"
  LIB_TYPE STATIC
  PROTO_DEPS ${PROTO_DEPS}
  SOURCES ${SOURCES}
  INSTALL_COMPONENT "${INSTALL_COMPONENT}"
)

if(INSTALL_COMPONENT)
  cpack_add_component("${INSTALL_COMPONENT}"
    DEPENDS core-proto
    GROUP ${INSTALL_GROUP}
    DISPLAY_NAME "Common Core Platform services - ProtoBuf/gRPC modules"
    DESCRIPTION "Generated runtime modules for communication with "
    "Common Core platform services over ProtoBuf/gRPC."
  )
endif()
