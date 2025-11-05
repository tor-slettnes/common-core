## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake recipe to install Python components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Create a library target to facilitate get_target_property()
set(TARGET "cc_core_python")

### Populate version/build info
configure_file(__init__.py.in "generated/__init__.py")
configure_file(buildinfo.py.in "generated/buildinfo.py")

### Modules to be installed as programs, with execute permissionss
set(PROGRAMS
)

### Files to be installed as is, without execute permissionss
set(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/generated/__init__.py
  ${CMAKE_CURRENT_BINARY_DIR}/generated/buildinfo.py
)

### Subdirectories
set(DIRECTORIES
  core
  protobuf
  messaging
  generated
)

### Wheel dependencies
cc_add_python("${TARGET}"
  PROTO_DEPS "cc_core_proto"
  PROGRAMS "${PROGRAMS}"
  FILES "${FILES}"
  DIRECTORIES "${DIRECTORIES}"
  INSTALL_CONDITION INSTALL_PYTHON_MODULES
  INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}"
)

cc_add_python_requirements_cache("core-python-requirements"
  REQUIREMENTS_FILES "${PYTHON_PIP_CORE_REQUIREMENTS_FILE}"
  INSTALL_CONDITION INSTALL_PYTHON_REQUIREMENTS
  INSTALL_COMPONENT "${PYTHON_REQUIREMENTS_INSTALL_COMPONENT}"
)

if(BUILD_SHARED_APPS)
  cc_add_python_wheel("core"
    PYTHON_DEPS "${TARGET}"
    DATA_DEPS "cc_core_settings"
    REQUIREMENTS_DEPS "core-python-requirements"
    DESCRIPTION "Python common components"
    INSTALL_CONDITION INSTALL_PYTHON_WHEELS
    INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}"
  )
endif()

cc_add_doxygen(python-reference
  FILE_PATTERNS "*.py;*.md"
)
