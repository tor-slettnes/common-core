## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

target_sources(${TARGET} PRIVATE
  logging.c++
)

add_subdirectory(message)
add_subdirectory(sinks)
add_subdirectory(dispatchers)
