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
  PACKAGE_COMPONENT "protobuf"
  INSTALL_COMPONENT "${INSTALL_COMPONENT}"
  FILES "${FILES}"
)
