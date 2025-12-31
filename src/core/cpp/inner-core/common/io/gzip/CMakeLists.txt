## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Common Core static/shared library
## @author Tor Slettnes
#===============================================================================

target_sources(${TARGET} PRIVATE
  base.c++
  reader.c++
  writer.c++
)

cc_add_package_dependencies(${TARGET}
  DEPENDS zlib
)
