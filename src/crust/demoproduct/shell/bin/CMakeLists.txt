## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake recipe to install binary scripts
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

cmake_minimum_required(VERSION 3.14)

if (BUILD_PYTHON)
  set(SOURCES demoshell)
  install(PROGRAMS ${SOURCES}
    TYPE BIN
    COMPONENT ${PKG_COMPONENT}
  )
endif()
