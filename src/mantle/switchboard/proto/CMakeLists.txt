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
set(SOURCES
  switchboard.proto
  )

### Generate language bindings
cc_add_proto("${TARGET}"
  PROTO_DEPS ${PROTO_DEPS}
  SOURCES ${SOURCES}
  PYTHON_INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}"
)
