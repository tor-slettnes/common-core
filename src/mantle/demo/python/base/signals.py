#!/bin/echo Do not invoke directly.
#===============================================================================
## @file signals.py
## @brief Local signal store for DEMO application
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install dir
from cc.protobuf.signal import CachingSignalStore
from ..protobuf import Signal

#===============================================================================
# Signal store to propagate `greeting` and `time` signals for application.

demo_signals = CachingSignalStore(signal_type = Signal)
