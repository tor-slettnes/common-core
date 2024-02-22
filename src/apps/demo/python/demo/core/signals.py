#!/bin/echo Do not invoke directly.
#===============================================================================
## @file demo.py
## @brief Demo - native Python implementation
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install dir
from messaging.protobuf.signalstore import SignalStore
from .types import CC

#===============================================================================
# Signal store to propagate `greeting` and `time` signals for application.

demo_signals = SignalStore(use_cache=True, signal_type = CC.Demo.Signal)
