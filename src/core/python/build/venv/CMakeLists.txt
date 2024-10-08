## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake recipe to create Python virtual environment
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET "python_venv")

find_package(Python3
  COMPONENTS Interpreter
)

add_custom_target("${TARGET}"
  DEPENDS "${PYTHON_VENV}/bin/pip")

add_custom_command(
  OUTPUT "${PYTHON_VENV}/bin/pip"
  COMMAND "${Python3_EXECUTABLE}"
  ARGS -m virtualenv -p "${Python3_EXECUTABLE}" "${PYTHON_VENV}"
  COMMENT "Creating Python Virual Environment: ${PYTHON_VENV}"
)

add_custom_command(
  OUTPUT "${PYTHON_VENV}/bin/pip"
  COMMAND "${PYTHON_VENV}/bin/python"
  ARGS -m pip install -r "${PYTHON_PIP_REQUIREMENTS_FILE}"
  APPEND
)
