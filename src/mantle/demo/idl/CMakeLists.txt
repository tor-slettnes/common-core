## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build language bindings from .idl files
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library. This can be used as a downstream dependency
set(TARGET cc_demo_idl)

### Static/shared library dependencies, either from this build or provided by
### the system. Only direct dependencies are needed.
set(LIB_DEPS cc_core_idl)

### Whether to generate example executables for each IDL file.
### With this option enabled, `rtiddsgen` will generate a publisher
### and subscriber pair for each recipe file without any interface
### definitions (as listed in the RECIPES variable, below), and
### and a client and service exeutable for those files that do
### have such a definition (as listed in the SERVICES variable).
set(GENERATE_EXAMPLE on)

### Sources containing IDL data types.  These should NOT contain
### any `interface` declarations, especially with the above
### GENERATE_EXAMPLE variable set.
set(RECIPES demo-types.idl)

### Sources containing DDS service interface.
### (an `interface` block with a `@DDSService` or `@service` decarator).
### This will:
###   - add an exertnal dependency on the appropriate RTI messaging
###     library (depending on whether the build type is Debug or Release)
###   - add `-client` and `-server` executable targets with the
###     GENERATE_EXAMPLES variable turned on.
set(SERVICES demo-interface.idl)

## Invoke common CMake rules
include(BuildIDL)
BuildIDL("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  RECIPES ${RECIPES}
  SERVICES ${SERVICES}
  GENERATE_EXAMPLE ${GENERATE_EXAMPLE}
)
