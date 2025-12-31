## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Common Core static/shared library
## @author Tor Slettnes
#===============================================================================

target_sources(${TARGET} PRIVATE
  init.c++
  host.c++
  logsink.c++
  path.c++
  process.c++
  runtime.c++
  serialport.c++
  symbols.c++
  systemservice.c++
  timezone.c++
  user.c++
  dns-sd.c++
)
