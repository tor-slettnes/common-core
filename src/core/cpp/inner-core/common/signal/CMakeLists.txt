## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Common Core static/shared library
## @author Tor Slettnes
#===============================================================================

target_sources(${TARGET} PRIVATE
  types.c++
  base-signal.c++
  void-signal.c++
  async-void-signal.c++
)
