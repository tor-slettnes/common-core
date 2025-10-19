## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build protocol buffer bindings
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Base name name of this target. A language speific suffix will be appended
### to the target library names (e.g. `_cpp`, `_py`).
set(TARGET "cc_platform_proto")

### Upstream ProtoBuf library dependencies.
set(PROTO_DEPS cc_core_proto)

### Source files
set(SOURCES
  cc/protobuf/sysconfig/sysconfig.proto
  cc/protobuf/netconfig/netconfig.proto
  cc/protobuf/vfs/vfs.proto
  cc/protobuf/upgrade/upgrade.proto
)

### Generate language bindings
cc_add_proto("${TARGET}"
  PROTOS ${SOURCES}
  BASE_DIR "${CC_PROTO_IMPORT_DIR}"
  PROTO_DEPS ${PROTO_DEPS}
  PYTHON_INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}"
)
