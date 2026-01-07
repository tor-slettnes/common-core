## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build protocol buffer bindings
## @author Tor Slettnes
#===============================================================================

### This can be used as a downstream dependency.
set(TARGET "cc_pubsub_proto")

### Static/shared library dependencies.
set(PROTO_DEPS cc_core_proto)

### Source files
set(pbuf_sources
  "cc/platform/pubsub/protobuf/pubsub_types.proto"
)

set(grpc_sources
  "cc/platform/pubsub/grpc/relay_service.proto"
)

### Generate language bindings
cc_add_proto("${TARGET}"
  PROTOBUF_SOURCES "${pbuf_sources}"
  GRPC_SOURCES "${grpc_sources}"
  IMPORT_DIRS "${CC_PROTOS_BASE_DIR}"
  PROTO_DEPS ${PROTO_DEPS}
  PYTHON_INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}"
)
