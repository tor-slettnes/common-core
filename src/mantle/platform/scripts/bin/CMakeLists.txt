## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake recipe to install binary scripts
## @author Tor Slettnes
#===============================================================================

if (BUILD_PYTHON)
  set(SOURCES platform-shell)

  if(PYTHON_INSTALL_COMPONENT)
    install(PROGRAMS ${SOURCES}
      TYPE BIN
      COMPONENT "${PYTHON_INSTALL_COMPONENT}"
    )
  endif()
endif()
