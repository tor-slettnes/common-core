## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(PKG_COMPONENT platform)

add_subdirectory(platformserver)
add_subdirectory(system-tool)
