## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

if(BUILD_PYTHON_IO)
  add_subdirectory(base)
  add_subdirectory(embedded)

  set(TARGET cc_core_io_python)

  set(LIB_DEPS
    cc_core_io_python_base
    cc_core_io_python_embedded
  )

  BuildLibrary("${TARGET}"
    LIB_TYPE INTERFACE
    LIB_DEPS ${LIB_DEPS}
  )
endif()

