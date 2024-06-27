## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake recipe to install binary scripts
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

if (BUILD_PYTHON)
  set(SOURCES shell)
  install(PROGRAMS ${SOURCES}
    TYPE BIN
    COMPONENT ${PKG_COMPONENT}
  )
endif()
