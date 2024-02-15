## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

target_sources(${TARGET} PRIVATE
  path.c++
  symbols.c++
  timezone.c++
  process.c++
  runtime.c++
  systemservice.c++
  logsink.c++
  init.c++
)
