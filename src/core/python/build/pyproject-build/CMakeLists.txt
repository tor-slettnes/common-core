## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake recipe to create Python virtual environment
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET "python_build")

add_custom_target("${TARGET}"
  DEPENDS "${PYTHON_VENV}/bin/pyproject-build")

add_dependencies("${TARGET}" "python_venv")

add_custom_command(
  OUTPUT "${PYTHON_VENV}/bin/pyproject-build"
  COMMAND "${PYTHON_VENV}/bin/python"
  ARGS -m pip install build
  WORKING_DIRECTORY "${PYTHON_VENV}"
)

