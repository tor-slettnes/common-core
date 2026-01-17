## -*- cmake -*-
#===============================================================================
## @file toolchain-Linux-aarch64.cmake
## @brief CMake toolchain file for Linux ARM64 targets
## @author Tor Slettnes
#===============================================================================

set(CMAKE_SYSTEM_NAME "Linux")
set(TOOLCHAIN_ARCH "aarch64-linux-gnu")

set(ENV{PKG_CONFIG_PATH} "/lib/${TOOLCHAIN_ARCH}/pkgconfig")

set(CMAKE_C_COMPILER ${TOOLCHAIN_ARCH}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_ARCH}-g++)
set(CMAKE_AR ${TOOLCHAIN_ARCH}-ar)
set(CMAKE_RANLIB ${TOOLCHAIN_ARCH}-ranlib)

set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "arm64")
