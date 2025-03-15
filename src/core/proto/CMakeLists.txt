## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build protocol buffer bindings
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### This can be used as a downstream dependency.
set(TARGET "cc_core_proto")

### Package component
if(BUILD_SHARED)
  set(INSTALL_COMPONENT "core-proto")
endif()

### Source files
set(SOURCES
  variant.proto
  status.proto
  signal.proto
  datetime.proto
  request_reply.proto
  version.proto
)

cc_add_proto("${TARGET}"
  LIB_TYPE STATIC
  SOURCES "${SOURCES}"
  INSTALL_COMPONENT "${INSTALL_COMPONENT}"
)

if(INSTALL_COMPONENT)
  cpack_add_component(${INSTALL_COMPONENT}
    GROUP ${INSTALL_GROUP}
    DISPLAY_NAME "Common ProtoBuf/gRPC modules"
    DESCRIPTION "Generated Protocol Buffers/gRPC modules for common components"
  )
endif()

