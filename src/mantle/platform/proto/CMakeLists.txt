## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build protocol buffer bindings
## @author Tor Slettnes
#===============================================================================

### Base name name of this target. A language speific suffix will be appended
### to the target library names (e.g. `_cpp`, `_py`).
set(TARGET "cc_platform_proto")

### Upstream ProtoBuf library dependencies.
set(PROTO_DEPS cc_core_proto)

### Source files
unset(pbuf_sources)
unset(grpc_sources)
foreach(service "sysconfig" "netconfig" "vfs" "upgrade")
  list(APPEND pbuf_sources "cc/platform/${service}/protobuf/${service}_types.proto")
  list(APPEND grpc_sources "cc/platform/${service}/grpc/${service}_service.proto")
endforeach()

### Generate language bindings
cc_add_proto("${TARGET}"
  PROTOBUF_SOURCES "${pbuf_sources}"
  GRPC_SOURCES "${grpc_sources}"
  IMPORT_DIRS "${CC_PROTOS_BASE_DIR}"
  PROTO_DEPS ${PROTO_DEPS}
  PYTHON_INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}"
)
