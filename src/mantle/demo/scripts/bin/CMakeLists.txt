## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake recipe to install binary scripts
## @author Tor Slettnes
#===============================================================================

if (BUILD_PYTHON)
  set(PROGRAMS
    ${EXEC_NAME_PREFIX}demo-shell
    ${EXEC_NAME_PREFIX}demo-grpc-aio-client-py
  )

  if(PYTHON_INSTALL_COMPONENT)
    install(
      PROGRAMS ${PROGRAMS}
      TYPE BIN
      COMPONENT "${PYTHON_INSTALL_COMPONENT}"
    )
  endif()
endif()
