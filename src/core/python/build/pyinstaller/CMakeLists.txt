## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake recipe to create Python virtual environment
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET "cc_python_pyinstaller")

add_custom_target("${TARGET}"
  DEPENDS "${PY_VENV_ROOT}/bin/pyinstaller")

add_dependencies("${TARGET}" "cc_python_venv")

add_custom_command(
  OUTPUT "${PY_VENV_ROOT}/bin/pyinstaller"
  COMMAND "${PY_VENV_ROOT}/bin/python"
  ARGS -m pip install PyInstaller
  WORKING_DIRECTORY "${PY_VENV_ROOT}"
)

