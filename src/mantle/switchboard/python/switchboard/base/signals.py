#!/bin/echo Do not invoke directly.
#===============================================================================
## @file signals.py
## @brief Local signal store for Swithboard application
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install dir
from cc.protobuf.signalstore import SignalStore
from cc.protobuf.switchboard import Signal

#===============================================================================
# Signal store to propagate `specification` and `status` signals

switchboard_signals = SignalStore(use_cache=True, signal_type = Signal)
