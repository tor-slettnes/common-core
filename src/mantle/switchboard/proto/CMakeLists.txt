## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to generate/build Switchboard ProtoBuf and gRPC bindings
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Base name name of this target. A language speific suffix will be appended
### to the target library names (e.g. `_cpp`, `_py`).
set(TARGET cc_switchboard_proto)

### Upstream ProtoBuf library dependencies.
set(PROTO_DEPS cc_core_proto)

### Source files
set(pbuf_sources
  "cc/platform/switchboard/protobuf/switchboard_types.proto"
)

set(grpc_sources
  "cc/platform/switchboard/grpc/switchboard_service.proto"
)

### Generate language bindings
cc_add_proto("${TARGET}"
  PROTOBUF_SOURCES "${pbuf_sources}"
  GRPC_SOURCES "${grpc_sources}"
  IMPORT_DIRS "${CC_PROTOS_BASE_DIR}"
  PROTO_DEPS ${PROTO_DEPS}
  PYTHON_INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}"
)
