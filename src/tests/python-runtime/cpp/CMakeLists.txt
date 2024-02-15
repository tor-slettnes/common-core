## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description Build main application
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this executable. This also becomes the name of the project.
set(TARGET pickletest)

### Libraries we depend on, either from this build or provided by the system.
set(LIB_DEPS
  cc_shared_ipc_python_embedded
  cc_shared_platform
)

### Source files
set(SOURCES
  main.c++
  )

## Invoke common CMake rules to build executable
include(BuildExecutable)
