#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file scalar_types.py
## @brief Convenience types for scalar values
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

class OCT8 (int):
    def __repr__ (self):
        return "0o%03o"%(self,)

class HEX8 (int):
    def __repr__ (self):
        return "0x%02X"%(self,)

class HEX16 (int):
    def __repr__ (self):
        return "0x%04X"%(self,)

class HEX32 (int):
    def __repr__ (self):
        return "0x%08X"%(self,)

class HEX64 (int):
    def __repr__ (self):
        return "0x%016X"%(self,)
