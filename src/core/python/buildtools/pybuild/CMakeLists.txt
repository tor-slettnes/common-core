## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake recipe to create Python virtual environment
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET "python_build")

get_target_property(TAG_DIR "python_venv" tag_dir)
set(BUILD_TAG "${TAG_DIR}/build")

add_custom_target("${TARGET}"
  DEPENDS ${BUILD_TAG})

add_custom_command(
  OUTPUT ${BUILD_TAG}
  DEPENDS "python_venv"
  COMMENT "Installing Python `build` tool: ${PYTHON_VENV}/bin/pyproject-build"

  COMMAND "${PYTHON_VENV}/bin/python"
  ARGS -m pip install --no-warn-script-location build

  COMMAND ${CMAKE_COMMAND}
  ARGS -E touch ${BUILD_TAG}
)

