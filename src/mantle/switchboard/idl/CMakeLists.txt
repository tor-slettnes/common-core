## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build language bindings from .idl files
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of library. Can be used as input/dependency for downstream targets
set(TARGET cc_switchboard_idl)

### Library dependencies
set(LIB_DEPS cc_core_idl)

### Sources containing IDL data types
set(RECIPES switchboard-types.idl)

### Sources containing IDL service definitions (these may overlap)
set(SERVICES switchboard-interface.idl)

### Whether or not to generate and build example code ("Modern C++" only).
set(GENERATE_EXAMPLE off)

### Include common recipe for generating code with RTI Connext DDS and then building.
include(build_idl)
cc_add_idl("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  RECIPES ${RECIPES}
  SERVICES ${SERVICES}
  GENERATE_EXAMPLE ${GENERATE_EXMAPLE}
)
