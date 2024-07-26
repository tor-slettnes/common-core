## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake recipe to install binary scripts
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

if (BUILD_PYTHON)
  set(SOURCES
    demoshell
    demo_grpc_aio_client_py
  )

  install(
    PROGRAMS ${SOURCES}
    TYPE BIN
    COMPONENT "${INSTALL_COMPONENT}"
  )
endif()
