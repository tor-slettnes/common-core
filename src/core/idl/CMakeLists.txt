## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build language bindings from .idl files
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of library. Can be used as input/dependency for downstream targets
set(TARGET cc_core_idl)

### Interface Description Language types
set(RECIPES
  common-types.idl
  variant-types.idl
  event-types.idl
)

### Include common recipe for generating code with RTI Connext DDS and then building.
include(build_idl)
cc_add_idl("${TARGET}"
  RECIPES ${RECIPES}
)
