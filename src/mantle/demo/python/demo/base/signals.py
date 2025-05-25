#!/bin/echo Do not invoke directly.
#===============================================================================
## @file signals.py
## @brief Local signal store for DEMO application
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install dir
from cc.protobuf.signalstore import SignalStore
from cc.protobuf.demo import Signal

#===============================================================================
# Signal store to propagate `greeting` and `time` signals for application.

demo_signals = SignalStore(use_cache=True, signal_type = Signal)
