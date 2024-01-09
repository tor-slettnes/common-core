##
## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake recipe to install binary scripts
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

cmake_minimum_required(VERSION 3.14)

if (USE_PYTHON)
  set(SOURCES shell)
  install(PROGRAMS ${SOURCES} TYPE BIN)
endif()
