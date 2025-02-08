## -*- cmake -*-
#===============================================================================
## @file toolchain-qnx.cmake
## @brief CMake toolchain file for QNX
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(PLATFORM_QNX ON)
add_compile_definitions(PLATFORM_QNX)

### Default architecture, if not set
set(QNX_PROCESSOR "x86_64"
  CACHE STRING "QNX target processor")

set(arch "gcc_nto${QNX_PROCESSOR}")
set(ntoarch "${QNX_PROCESSOR}")

set(CMAKE_SYSTEM_NAME QNX)
set(CMAKE_SYSTEM_PROCESSOR ${QNX_PROCESSOR})

# set(QNX_FLAGS "-Wall -fmessage-length=0")
# set(QNX_FLAGS "${QNX_FLAGS} -g -O0 -fno-builtin")

set(CMAKE_C_COMPILER qcc)
set(CMAKE_C_COMPILER_TARGET ${arch})
#set(CMAKE_CXX_FLAGS "-V${arch} ${QNX_FLAGS}" CACHE STRING "")

set(CMAKE_CXX_COMPILER q++)
set(CMAKE_CXX_COMPILER_TARGET ${arch})
#set(CMAKE_CXX_FLAGS "-V${arch} ${QNX_FLAGS}" CACHE STRING "")

set(CMAKE_ASM_COMPILER qcc -V${arch})
set(CMAKE_ASM_DEFINE_FLAG "-Wa,--defsym,")

set(CMAKE_RANLIB nto${ntoarch}-ranlib)
set(CMAKE_AR nto${ntoarch}-ar)

set(CMAKE_SYSROOT $ENV{QNX_TARGET})

### Ensure that the `q++` compiler recognizes C++ files regardless of suffix
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-xc++>)
