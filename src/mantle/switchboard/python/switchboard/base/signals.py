#!/bin/echo Do not invoke directly.
#===============================================================================
## @file signals.py
## @brief Local signal store for Swithboard application
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install dir
from cc.protobuf.signal import SignalStore
from cc.protobuf.switchboard import Signal

#===============================================================================
# Signal store to propagate `specification` and `status` signals

signal_store = SignalStore(use_cache=True, signal_type = Signal)
