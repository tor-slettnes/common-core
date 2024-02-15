## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

if(BUILD_PYTHON_CPP)
  add_subdirectory(base)
  add_subdirectory(embedded)

  set(TARGET cc_shared_messaging_python)
  set(LIB_TYPE INTERFACE)
  set(LIB_DEPS
    cc_shared_messaging_python_base
    cc_shared_messaging_python_embedded
  )
  include(BuildLibrary)

endif()

