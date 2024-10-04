## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules for Python modules
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Create a custom target for the Python Wheel
set(TARGET "cc_demo_python_protobuf")

set(FILES
  demo.py
)

BuildPython("${TARGET}"
  NAMESPACE_COMPONENT "protobuf"
  INSTALL_COMPONENT "${INSTALL_COMPONENT}"
  PYTHON_DEPS cc_core_python
  FILES "${FILES}"
)
