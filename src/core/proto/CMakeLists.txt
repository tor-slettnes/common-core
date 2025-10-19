## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build protocol buffer bindings
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### This can be used as a downstream dependency.
set(TARGET "cc_core_proto")

### Source files
set(SOURCES
  variant.proto
  status.proto
  signal.proto
  datetime.proto
  quantities.proto
  request_reply.proto
  version.proto
)

list(TRANSFORM SOURCES
  PREPEND "cc/protobuf/core/")

cc_add_proto("${TARGET}"
  PROTOS "${SOURCES}"
  BASE_DIR "${CC_PROTO_IMPORT_DIR}"
  PYTHON_INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}"
)
