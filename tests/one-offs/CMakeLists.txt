## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes
#===============================================================================

if(BUILD_PYTHON_IO)
  add_subdirectory(python-runtime)
endif()

if(BUILD_HTTP)
  add_subdirectory(rest-tool)
endif()

add_subdirectory(json-parser)
add_subdirectory(dt-parser)
