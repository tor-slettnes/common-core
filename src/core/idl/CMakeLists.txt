## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build language bindings from .idl files
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
include(BuildIDL)
BuildIDL("${TARGET}"
  LIB_TYPE STATIC
  RECIPES ${RECIPES}
)
