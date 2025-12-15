## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake recipe to install binary scripts
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

if (BUILD_PYTHON)
  set(PROGRAMS
    demo-shell
  )

  if(PYTHON_INSTALL_COMPONENT)
    foreach(source ${SOURCES})
      install(PROGRAMS ${source}
        TYPE BIN
        RENAME "${ORGANIZATION}-${source}"
        COMPONENT "${PYTHON_INSTALL_COMPONENT}"
      )
    endforeach()
  endif()
endif()
