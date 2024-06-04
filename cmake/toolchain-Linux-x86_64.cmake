## -*- cmake -*-
#===============================================================================
## @file toolchain-Linux-x86_64.cmake
## @brief CMake toolchain file for Linux x86_64 targets
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(LINUX YES)

string(TOLOWER "$<CONFIG>" buildconfig)
if("${buildconfig}" STREQUAL "debug")
  add_compile_options(-Wall -Wextra -Wno-unused -pedantic)
endif()

if(USE_STATIC_LIBS)
  add_link_options(-static)
endif()
