## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake recipe to create Python virtual environment
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET "python_pyinstaller")

get_target_property(TAG_DIR "python_venv" tag_dir)
set(INSTALLER_TAG "${TAG_DIR}/pyinstaller")

add_custom_target("${TARGET}"
  DEPENDS ${INSTALLER_TAG})

add_custom_command(
  OUTPUT ${INSTALLER_TAG}
  DEPENDS "python_venv"
  COMMENT "Installing Python `PyInstaller` tool: ${PYTHON_VENV}/bin/pyinstaller"

  COMMAND "${PYTHON_VENV}/bin/python"
  ARGS -m pip install --no-warn-script-location PyInstaller

  COMMAND ${CMAKE_COMMAND}
  ARGS -E touch ${INSTALLER_TAG}
)
