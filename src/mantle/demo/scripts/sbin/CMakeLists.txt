## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake recipe to install binary scripts
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

if (BUILD_PYTHON)
  set(SOURCES
    demo_grpc_server_py
    demo_grpc_aio_server_py
  )

  install(PROGRAMS ${SOURCES}
    TYPE SBIN
    COMPONENT "${INSTALL_COMPONENT}"
  )
endif()
