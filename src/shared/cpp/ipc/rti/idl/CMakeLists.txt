##
## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief Build IDL encoder/decoder methods for share data types
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET cc_shared_ipc_rti_idl)

### Library type.  We use static linking for convenience.
set(LIB_TYPE STATIC)

### Library dependencies.
set(LIB_DEPS
  cc_shared_idl
  cc_shared_platform)

set(SOURCES
  translate-idl-variant.c++
  translate-idl-common.c++
  translate-idl-eventtypes.c++
)

### Recepies to build the library
include(BuildLibrary)
