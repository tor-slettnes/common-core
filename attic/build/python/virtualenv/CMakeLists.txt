## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief Create Python virtual environment
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

#-------------------------------------------------------------------------------
# Create Python `virtualenv`

set(STAMP_DIR "${PYTHON_VENV}/build-stamps")
set(VENV_TARGET "python_venv")
set(VENV_STAMP "${STAMP_DIR}/created")

add_custom_target(${VENV_TARGET}
  DEPENDS ${VENV_STAMP}
)

set_target_properties(${VENV_TARGET} PROPERTIES
  stamp_dir ${STAMP_DIR})


add_custom_command(
  OUTPUT "${VENV_STAMP}"
  COMMENT "Creating Python Virtual Environment: ${PYTHON_VENV}"
  # BYPRODUCTS ${PYTHON_VENV}

  COMMAND ${Python3_EXECUTABLE}
  ARGS -m virtualenv -p "${Python3_EXECUTABLE}" "${PYTHON_VENV}"

  COMMAND ${CMAKE_COMMAND}
  ARGS -E make_directory ${STAMP_DIR}

  COMMAND ${CMAKE_COMMAND}
  ARGS -E touch ${VENV_STAMP}
)

