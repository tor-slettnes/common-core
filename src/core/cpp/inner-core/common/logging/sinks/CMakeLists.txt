## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

target_sources(${TARGET} PRIVATE
  rotatingpath.c++
  messageformatter.c++
  logsink.c++
  asynclogsink.c++
  streamsink.c++
  logfilesink.c++
  jsonfilesink.c++
  tabulardatasink.c++
  csvfilesink.c++
)
