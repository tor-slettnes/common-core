## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules for Python modules
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Create a custom target for the Python Wheel
set(TARGET "cc_platform_python_protobuf")

set(FILES
  vfs.py
  sysconfig.py
  netconfig.py
  upgrade.py
)

cc_add_python("${TARGET}"
  NAMESPACE_COMPONENT "protobuf"
  INSTALL_COMPONENT "${INSTALL_COMPONENT}"
  PYTHON_DEPS "cc_core_python"
  PROTO_DEPS "cc_platform_proto"
  FILES "${FILES}"
)
