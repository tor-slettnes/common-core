## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake recipe to create Python virtual environment
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET "python_pyinstaller")

add_custom_target("${TARGET}"
  DEPENDS "${PYTHON_VENV}/bin/pyinstaller")

add_dependencies("${TARGET}" "python_venv")

add_custom_command(
  OUTPUT "${PYTHON_VENV}/bin/pyinstaller"
  COMMAND "${PYTHON_VENV}/bin/python"
  ARGS -m pip install PyInstaller
  WORKING_DIRECTORY "${PYTHON_VENV}"
)

