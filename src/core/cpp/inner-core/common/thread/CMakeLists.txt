## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Common Core static/shared library
## @author Tor Slettnes
#===============================================================================

target_sources(${TARGET} PRIVATE
  signaltemplate.c++
  binaryevent.c++
  blockingqueue.c++
  uniquelock.c++
)
