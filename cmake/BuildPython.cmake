## -*- cmake -*-
#===============================================================================
## @file BuildPython.cmake
## @brief CMake include file to "build" (install) Python modules
## @author Tor Slettnes <tor@slett.net>
#===============================================================================
##
## To use this file, copy, uncomment, and modify the following in your "CMakeLists.txt":
##
## @code
##     ### Modules to be installed as programs, with execute permissionss
##     set(PROGRAMS my_main_module.py ...)
##
##     ### Modules to be installed as plain files, without execute permissions
##     set(FILES my_supporting_module.py ...)
##
##     ## Subdirectories
##     set(DIRECTORIES my_subdirectory ...)
##
##     ## Invoke common installation rules
##     include(BuildPython)
## @endcode

if (BUILD_PYTHON)
  if(NOT PYTHON_INSTALL_DIR)
    set(PYTHON_INSTALL_DIR "share/python")
  endif()

  if(PROGRAMS)
    install(
      PROGRAMS ${PROGRAMS}
      DESTINATION ${PYTHON_INSTALL_DIR})
  endif()

  if(FILES)
    install(
      FILES ${FILES}
      DESTINATION ${PYTHON_INSTALL_DIR})
  endif()

  if(DIRECTORIES)
    install(
      DIRECTORY ${DIRECTORIES}
      DESTINATION ${PYTHON_INSTALL_DIR}
      PATTERN __pycache__ EXCLUDE)
  endif()
endif()

