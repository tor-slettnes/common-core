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
  cc/platform/sysconfig/sysconfig.proto
  cc/platform/netconfig/netconfig.proto
  cc/platform/vfs/vfs.proto
  cc/platform/upgrade/upgrade.proto
)

### Generate language bindings
cc_add_proto("${TARGET}"
  PROTOS ${SOURCES}
  IMPORT_DIRS "${CC_PROTOS_BASE_DIR}"
  PROTO_DEPS ${PROTO_DEPS}
  PYTHON_INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}"
)
