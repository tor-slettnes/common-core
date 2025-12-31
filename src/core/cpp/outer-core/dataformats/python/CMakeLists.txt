## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build static/shared library
## @author Tor Slettnes
#===============================================================================

if(BUILD_PYTHON_IO)
  add_subdirectory(base)
  add_subdirectory(embedded)

  set(TARGET cc_core_io_python)

  set(LIB_DEPS
    cc_core_io_python_base
    cc_core_io_python_embedded
  )

  cc_add_library("${TARGET}"
    LIB_TYPE INTERFACE
    LIB_DEPS ${LIB_DEPS}
  )
endif()

