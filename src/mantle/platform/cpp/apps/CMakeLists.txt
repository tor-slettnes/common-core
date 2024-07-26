## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(INSTALL_COMPONENT platform)

add_subdirectory(platformserver)
add_subdirectory(system-tool)
add_subdirectory(network-tool)
add_subdirectory(vfs-tool)
