## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules for Python modules
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Create a custom target for the Python Wheel
set(TARGET "cc_switchboard_python_protobuf")

set(FILES
  switchboard.py
)

cc_add_python("${TARGET}"
  NAMESPACE_COMPONENT "protobuf"           # Python 2nd level namespace component
  INSTALL_COMPONENT "${INSTALL_COMPONENT}" # Debian package component
  PYTHON_DEPS "cc_core_python"             # Python dependencies
  PROTO_DEPS "cc_switchboard_proto"        # ProtoBuf dependencies
  FILES "${FILES}"
)
