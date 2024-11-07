#!/bin/echo Do not invoke directly.
#===============================================================================
## @file signals.py
## @brief Local signal store for DEMO application
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install dir
from cc.protobuf.signal import SignalStore
from cc.protobuf.demo import Signal

#===============================================================================
# Signal store to propagate `greeting` and `time` signals for application.

signal_store = SignalStore(use_cache=True, signal_type = Signal)
