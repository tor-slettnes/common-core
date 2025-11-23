#!/bin/echo Do not invoke directly.
#===============================================================================
## @file signals.py
## @brief Local signal store for Swithboard application
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

from cc.protobuf.signal import CachingSignalStore
from ..protobuf import Signal

#===============================================================================
# Signal store to propagate `specification` and `status` signals

switchboard_signals = CachingSignalStore(signal_type = Signal)
