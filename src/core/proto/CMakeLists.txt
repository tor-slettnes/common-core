## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build protocol buffer bindings
## @author Tor Slettnes
#===============================================================================

### This can be used as a downstream dependency.
set(TARGET "cc_core_proto")

### Source files
set(SOURCES
  cc/protobuf/variant/variant.proto
  cc/protobuf/status/status.proto
  cc/protobuf/signal/signal.proto
  cc/protobuf/datetime/datetime.proto
  cc/protobuf/quantities/quantities.proto
  cc/protobuf/request_reply/request_reply.proto
  cc/protobuf/version/version.proto
)

cc_add_proto("${TARGET}"
  PROTOBUF_SOURCES "${SOURCES}"
  IMPORT_DIRS "${CC_PROTOS_BASE_DIR}"
  PYTHON_INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}"
)
