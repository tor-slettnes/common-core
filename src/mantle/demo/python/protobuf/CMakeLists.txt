## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules for Python modules
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Create a custom target for the Python Wheel
set(TARGET "cc_demo_protobuf")

set(FILES
  demo.py
)

BuildPython("${TARGET}"
  PACKAGE "${CPACK_PYTHON_PACKAGE}.protobuf"
  COMPONENT "${PKG_COMPONENT}"
  FILES "${FILES}"
)
