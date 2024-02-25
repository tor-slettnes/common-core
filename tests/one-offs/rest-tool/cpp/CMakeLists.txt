## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build DDS Demo tool
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this executable. This also becomes the name of the project.
set(TARGET rest-tool)

### Libraries we depend on, either from this build or provided by the
### system.
set(LIB_DEPS
  cc_core_platform
  cc_core_messaging_rest
)

### Source files
set(SOURCES
  main.c++
  )

## Invoke common CMake rules to build executable
include(BuildExecutable)
