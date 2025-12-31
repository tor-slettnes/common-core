## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Common Core static/shared library
## @author Tor Slettnes
#===============================================================================

target_sources(${TARGET} PRIVATE
  rotatingpath.c++
  tabulardata.c++
  sink.c++
  messagesink.c++
  csvfilesink.c++
  jsonfilesink.c++
  logfilesink.c++
  streamsink.c++
  factory.c++
)
