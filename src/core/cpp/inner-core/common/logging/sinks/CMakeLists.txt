## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

target_sources(${TARGET} PRIVATE
  rotatingpath.c++
  tabulardata.c++
  messageformatter.c++
  sink.c++
  factory.c++
  logsink.c++
  asynclogsink.c++
  streamsink.c++
  logfilesink.c++
  jsonfilesink.c++
  csvfilesink.c++
)
