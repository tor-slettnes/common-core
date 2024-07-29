## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(INSTALL_COMPONENT platform)

add_subdirectory(platformserver)
add_subdirectory(sysconfig-tool)
add_subdirectory(netconfig-tool)
add_subdirectory(vfs-tool)
