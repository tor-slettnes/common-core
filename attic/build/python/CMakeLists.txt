## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake recipe to create Python virtual environment
## @author Tor Slettnes
#===============================================================================

if(BUILD_PYTHON)
  find_package(Python3
    COMPONENTS Interpreter
    REQUIRED
  )

  # add_subdirectory(requirements)
  # add_subdirectory(virtualenv)
  # add_subdirectory(runtime)
  # add_subdirectory(pybuild)
  #add_subdirectory(pyinstaller)
endif()
