## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build protocol buffer bindings
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### This can be used as a downstream dependency.
set(TARGET cc_core_proto)

### Source files
set(SOURCES
  variant_types.proto
  event_types.proto
  signal_types.proto
  request_reply.proto
)

### Generate language bindings
include(BuildProto)
BuildProto("${TARGET}"
  LIB_TYPE STATIC
  SOURCES "${SOURCES}"
)
