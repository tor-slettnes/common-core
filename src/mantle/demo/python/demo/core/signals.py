#!/bin/echo Do not invoke directly.
#===============================================================================
## @file signals.py
## @brief Local signal store for DEMO application
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install dir
from cc.protobuf.signal import SignalStore
from cc.protobuf.import_proto import import_proto

## Import symbols generted from `demo.proto`. These will appear in
## the namespace corresponding to its package name, `cc.demo`.
import_proto('demo', globals())

#===============================================================================
# Signal store to propagate `greeting` and `time` signals for application.

signal_store = SignalStore(use_cache=True, signal_type = cc.demo.Signal)
