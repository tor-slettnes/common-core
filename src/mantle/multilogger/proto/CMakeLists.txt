## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build protocol buffer bindings
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### This can be used as a downstream dependency.
set(TARGET "cc_multilogger_proto")

### Package component
if(INCLUDE_LOGGER)
  set(INSTALL_COMPONENT "multilogger-proto")
endif()

### Static/shared library dependencies.
set(PROTO_DEPS cc_core_proto)

### Source files
set(SOURCES
  multilogger.proto
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
    DISPLAY_NAME "Common Core Log service - ProtoBuf/gRPC modules"
    DESCRIPTION "Generated runtime modules for communication with "
    "Common Core log service over ProtoBuf/gRPC."
  )
endif()
