## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules for Python modules
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Create a custom target for the Python Wheel
set(TARGET "cc_logger_python_protobuf")

set(FILES
  logger.py
)

cc_add_python("${TARGET}"
  NAMESPACE_COMPONENT "protobuf"           # Python 2nd level namespace component
  INSTALL_COMPONENT "${INSTALL_COMPONENT}" # Debian package component
  PYTHON_DEPS "cc_core_python"             # Python dependencies
  PROTO_DEPS "cc_logger_proto"             # ProtoBuf dependencies
  FILES "${FILES}"
)
