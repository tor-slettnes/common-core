## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake recipe to install binary scripts
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

if (BUILD_PYTHON)
  set(PROGRAMS
    demoshell
    demo_grpc_aio_client_py
  )

  if(PYTHON_INSTALL_COMPONENT)
    install(
      PROGRAMS ${PROGRAMS}
      TYPE BIN
      COMPONENT "${PYTHON_INSTALL_COMPONENT}"
    )
  endif()
endif()
