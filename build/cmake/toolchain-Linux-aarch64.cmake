## -*- cmake -*-
#===============================================================================
## @file toolchain-Linux-aarch64.cmake
## @brief CMake toolchain file for Linux ARM64 targets
## @author Tor Slettnes
#===============================================================================

set(LINUX YES)

string(TOLOWER "$<CONFIG>" buildconfig)
if("${buildconfig}" STREQUAL "debug")
  add_compile_options(-Wall -Wextra -Wno-unused -pedantic)
endif()

#set(THREADS_PREFER_PTHREAD_FLAG ON)

# Configure C++ filesystem library
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lstdc++fs")
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lstdc++fs")
# set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -lstdc++fs")

set(TOOLCHAIN_PREFIX aarch64-linux-gnu)

set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)
set(CMAKE_AR ${TOOLCHAIN_PREFIX}-ar)
set(CMAKE_RANLIB ${TOOLCHAIN_PREFIX}-ranlib)
