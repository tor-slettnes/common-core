## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake recipe to create Python virtual environment
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET "python_pyinstaller")

find_package(Python3
  COMPONENTS Interpreter
  REQUIRED
)

get_target_property(STAMP_DIR python_venv stamp_dir)

set(INSTALL_STAMP  "${STAMP_DIR}/pyinstaller")

add_custom_target("${TARGET}"
  DEPENDS ${INSTALL_STAMP})

add_custom_command(
  OUTPUT ${INSTALL_STAMP}
  DEPENDS "python_venv"
  COMMENT "Installing Python `PyInstaller` tool"

  COMMAND "${PYTHON_VENV}/bin/python"
  ARGS -m pip install --no-warn-script-location PyInstaller

  COMMAND ${CMAKE_COMMAND}
  ARGS -E touch ${INSTALL_STAMP}
)
