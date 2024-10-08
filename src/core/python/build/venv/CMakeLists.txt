## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake recipe to create Python virtual environment
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET "cc_python_venv")

set(PY_VENV_ROOT "${CMAKE_SOURCE_DIR}/venv"
  CACHE STRING "Python Virtual Environment Root Folder")

find_package(Python3
  COMPONENTS Interpreter
)

add_custom_target(cc_python_venv
  DEPENDS "${PY_VENV_ROOT}/bin/pip")

add_custom_command(
  OUTPUT "${PY_VENV_ROOT}/bin/pip"
  COMMAND "${Python3_EXECUTABLE}"
  ARGS -m virtualenv -p "${Python3_EXECUTABLE}" "${PY_VENV_ROOT}"
  COMMENT "Creating Python Virual Environment: ${PY_VENV_ROOT}"
)


get_build_arg(PYTHON_PIP_REQUIREMENTS_FILE
  SETTING "python" "pip requirements file"
  TYPE FILEPATH
  DOCSTRING "Package requirements file for Python Virtual Environment"
  DEFAULT "${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt"
)

cmake_path(APPEND CMAKE_SOURCE_DIR "${PYTHON_PIP_REQUIREMENTS_FILE}"
  OUTPUT_VARIABLE requirements_file)


add_custom_command(
  OUTPUT "${PY_VENV_ROOT}/bin/pip"
  COMMAND "${PY_VENV_ROOT}/bin/python"
  ARGS -m pip install -r "${requirements_file}"
  APPEND
)
