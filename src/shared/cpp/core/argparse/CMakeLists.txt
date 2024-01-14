## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

target_sources(${TARGET} PUBLIC
  option.c++
  parser.c++
  common.c++
  server.c++
  client.c++
  command.c++
)
