## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

target_sources(${TARGET} PRIVATE
  rotatingpath.c++
  logsink.c++
  messagesink.c++
  logfilesink.c++
  jsonfilesink.c++
  streamsink.c++
)
