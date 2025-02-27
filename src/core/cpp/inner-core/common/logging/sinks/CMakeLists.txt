## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

target_sources(${TARGET} PRIVATE
  capture.c++
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
