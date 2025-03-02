## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake recipe to create Python virtual environment
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET "python_venv")
set(TAG_DIR "${PYTHON_VENV}/tags")
set(REQUIREMENTS_TAG "${TAG_DIR}/requirements")
set(VENV_TAG "${PYTHON_VENV}/bin/pip")

find_package(Python3
  COMPONENTS Interpreter
  REQUIRED
)

add_custom_target(${TARGET}
  DEPENDS ${VENV_TAG} ${REQUIREMENTS_TAG}
)

set_target_properties(${TARGET}
  PROPERTIES tag_dir "${TAG_DIR}")

add_custom_command(
  OUTPUT "${VENV_TAG}" "${TAG_DIR}"
  COMMENT "Creating Python Build Environment: ${PYTHON_VENV}"
  # BYPRODUCTS ${PYTHON_VENV}

  COMMAND ${Python3_EXECUTABLE}
  ARGS -m virtualenv -p "${Python3_EXECUTABLE}" "${PYTHON_VENV}"

  COMMAND ${CMAKE_COMMAND}
  ARGS -E make_directory "${TAG_DIR}"
)

add_custom_command(
  OUTPUT ${REQUIREMENTS_TAG}
  DEPENDS "${PYTHON_PIP_REQUIREMENTS_FILE}" "${VENV_TAG}"
  COMMENT "Installing Python packages from: ${PYTHON_PIP_REQUIREMENTS_FILE}"

  COMMAND "${PYTHON_VENV}/bin/python"
  ARGS -m pip install --no-warn-script-location -r "${PYTHON_PIP_REQUIREMENTS_FILE}"

  COMMAND ${CMAKE_COMMAND}
  ARGS -E touch ${REQUIREMENTS_TAG}
)
