## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build protocol buffer bindings
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Base name name of this target. A language speific suffix will be appended
### to the target library names (e.g. `_cpp`, `_py`).
set(TARGET "cc_platform_proto")

### Package component
if(BUILD_PLATFORM)
  set(INSTALL_COMPONENT "platform-proto")
endif()

### Upstream ProtoBuf library dependencies.
set(PROTO_DEPS cc_core_proto)

### Source files
set(SOURCES
  sysconfig.proto
  netconfig.proto
  vfs.proto
  upgrade.proto
)

### Generate language bindings
cc_add_proto("${TARGET}"
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
